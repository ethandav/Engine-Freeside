#include "..\..\include\d3d12\passes\D3D12ForwardLitGeometryRenderPass.h"
#include <algorithm>

namespace efg::d3d12
{
    void D3D12ForwardLitGeometryRenderPass::Initialize(D3D12GraphicsPipelineLibary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12MaterialLibrary* materialLibrary, D3D12BufferFactory* bufferFactory)
    {
        m_pipelineLibrary = pipelineLib;
        m_descriptorContext = descriptorCtx;
        m_meshLibrary = meshLibrary;
        m_materialLibrary = materialLibrary;
        m_bufferFactory = bufferFactory;

        CreatePassResources();
    }

    void D3D12ForwardLitGeometryRenderPass::Execute(const FrameContext& ctx, const FramePacket& scene)
	{
        BeginPass(ctx);
        UploadPassResources(ctx, scene);
        BindPassResources(ctx);
        DrawAllRenderObjects(ctx, scene);
	}

    void D3D12ForwardLitGeometryRenderPass::CreatePassResources()
    {

    }

    void D3D12ForwardLitGeometryRenderPass::BeginPass(const FrameContext& ctx)
    {
        ctx.frame->ResetTransientAllocators();
        m_passResources[ctx.frameIndex].ResetTransientAllocators();
    }

    void D3D12ForwardLitGeometryRenderPass::UploadPassResources(const FrameContext& ctx, const FramePacket& scene)
    {
        UploadPointLights(ctx, scene);
        UploadFrameConstants(ctx, scene);
    }

    void D3D12ForwardLitGeometryRenderPass::BindPassResources(const FrameContext& ctx)
    {
        m_pipelineLibrary->BindPipeline(ctx.commandList, PipelineId::ForwardLitGeometry);
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), m_cameraCB);
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLight), m_directionalLightCB);
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::PointLightConstants), m_pointLightCB);
        ctx.commandList->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::PointLightsSrv), m_pointLightsSB.gpu);
    }

    void D3D12ForwardLitGeometryRenderPass::UploadFrameConstants(const FrameContext& ctx, const FramePacket& scene)
    {
        CameraConstants cameraConstants = scene.camera.BuildCameraConstants();
        Lights::DirectionalLightConstants dirLightConstants = scene.directionalLight.BuildDirectionalLightConstants();
        m_cameraCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &cameraConstants, sizeof(CameraConstants));
        m_directionalLightCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &dirLightConstants, sizeof(Lights::DirectionalLightConstants));
        m_pointLightCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &m_pointLightConstants, sizeof(Lights::PointLightConstants));
    }

    void D3D12ForwardLitGeometryRenderPass::UploadPointLights(const FrameContext& ctx, const FramePacket& scene)
    {
        uint32_t count = 0;

        if (!scene.pointLights.empty())
        {
            count = scene.pointLights.size();
            m_pointLightsSB = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, count * sizeof(Lights::GpuPointLight), InstanceDataAlignment);
            Lights::GpuPointLight* instances = reinterpret_cast<Lights::GpuPointLight*>(m_pointLightsSB.cpu);

            for (uint32_t i = 0; i < count; ++i)
            {
                const Lights::Point& light = (scene.pointLights)[i];

                instances[i].positionAndRadius = {
                    light.position.x,
                    light.position.y,
                    light.position.z,
                    light.radius
                };

                instances[i].colorAndIntensity = {
                    light.color.x,
                    light.color.y,
                    light.color.z,
                    light.intensity
                };
            }
        }

        m_pointLightConstants.pointLightCount = count;
    }

    void D3D12ForwardLitGeometryRenderPass::DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene)
    {
        std::vector<uint32_t> sortedIndices(scene.renderObjects.size());

        SortRenderObjectsForInstancing(sortedIndices, scene);
        uint32_t begin = 0;

        while (begin < sortedIndices.size())
        {
            const RenderObject& first = scene.renderObjects[sortedIndices[begin]];

            uint32_t end = begin + 1;

            while (end < sortedIndices.size())
            {
                const RenderObject& candidate = scene.renderObjects[sortedIndices[end]];

                if (candidate.mesh != first.mesh ||
                    candidate.material != first.material)
                {
                    break;
                }

                ++end;
            }

            // Objects in [begin, end) are one batch.
            DrawInstancedBatch(ctx, scene, sortedIndices, begin, end);

            begin = end;
        }
    }

    void D3D12ForwardLitGeometryRenderPass::SortRenderObjectsForInstancing(std::vector<uint32_t>& sortedIndices, const FramePacket& scene)
    {
        for (uint32_t i = 0; i < sortedIndices.size(); ++i)
        {
            sortedIndices[i] = i;
        }

        std::sort(sortedIndices.begin(), sortedIndices.end(),
            [&](uint32_t lhs, uint32_t rhs)
            {
                const RenderObject& a = scene.renderObjects[lhs];
                const RenderObject& b = scene.renderObjects[rhs];

                if (a.material.index != b.material.index)
                    return a.material.index < b.material.index;

                return a.mesh.index < b.mesh.index;
            });
    }

    void D3D12ForwardLitGeometryRenderPass::DrawInstancedBatch(const FrameContext& ctx, const FramePacket& scene, const std::vector<uint32_t>& sortedIndices, uint32_t begin, uint32_t end)
    {
        const RenderObject& first = scene.renderObjects[sortedIndices[begin]];
        const GpuMaterial& material = first.material.IsValid() ? m_materialLibrary->GetMaterialByHandle(first.material) : m_materialLibrary->GetDefaultMaterial();
        D3D12_GPU_VIRTUAL_ADDRESS materialCbAddress = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &material, sizeof(GpuMaterial));
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), materialCbAddress);
        const uint32_t instanceCount = end - begin;
        const UINT64 instanceBufferSize = static_cast<UINT64>(instanceCount) * sizeof(InstanceData);
        GpuUploadBufferAllocation instanceAllocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, instanceBufferSize, InstanceDataAlignment);
        InstanceData* instances = reinterpret_cast<InstanceData*>(instanceAllocation.cpu);

        for (uint32_t i = 0; i < instanceCount; ++i)
        {
            const RenderObject& object = scene.renderObjects[sortedIndices[begin + i]];
            instances[i] = {};
            instances[i].world = efg::Math::Transpose(object.world);
        }

        ctx.commandList->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::InstanceData), instanceAllocation.gpu);
        DrawMeshInstanced(ctx.commandList, first.mesh, instanceCount);
    }

    void D3D12ForwardLitGeometryRenderPass::DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, efg::MeshHandle handle, uint32_t instanceCount)
    {
        const GpuMesh& mesh = m_meshLibrary->Get(handle);
        commandList->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);
        if (mesh.indexCount > 0)
        {
            commandList->IASetIndexBuffer(&mesh.indexBufferView);
            commandList->DrawIndexedInstanced(mesh.indexCount, instanceCount, 0, 0, 0);

        }
        else
        {
            commandList->DrawInstanced(mesh.vertexCount, instanceCount, 0, 0);
        }
    }
}