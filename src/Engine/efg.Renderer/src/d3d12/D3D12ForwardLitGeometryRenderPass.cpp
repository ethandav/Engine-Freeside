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
    }

    void D3D12ForwardLitGeometryRenderPass::Execute(const FrameContext& ctx, const FramePacket& scene)
	{
        UpdatePointLights(ctx, scene);
        UpdateFrameConstants(ctx, scene);
        m_pipelineLibrary->BindPipeline(ctx.commandList, PipelineId::ForwardLitGeometry);
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), ctx.frame->cameraConstantBuffer.resource->GetGPUVirtualAddress());
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLight), ctx.frame->directionalLightConstantBuffer.resource->GetGPUVirtualAddress());
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::PointLightConstants), ctx.frame->pointLightConstantBuffer.resource->GetGPUVirtualAddress());

        ID3D12DescriptorHeap* heaps[] = { m_descriptorContext->GetCBVSRVUAVHeap() };
        ctx.commandList->SetDescriptorHeaps(_countof(heaps), heaps);
        ctx.commandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(ForwardLitRootParameter::PointLightsSrv), ctx.frame->pointLightStructuredBuffer.gpuSrv);
        DrawAllRenderObjects(ctx, scene);
	}

    void D3D12ForwardLitGeometryRenderPass::UpdateFrameConstants(const FrameContext& ctx, const FramePacket& scene)
    {
        CameraConstants cameraConstants = scene.camera.BuildCameraConstants();
        Lights::DirectionalLightConstants dirLightConstants = scene.directionalLight.BuildDirectionalLightConstants();
        ctx.frame->objectConstantArena.Reset();
        ctx.frame->materialConstantArena.Reset();
        ctx.frame->gpuUploadBufferArena.Reset();
        m_bufferFactory->UpdateConstantBuffer(ctx.frame->cameraConstantBuffer, &cameraConstants, sizeof(CameraConstants));
        m_bufferFactory->UpdateConstantBuffer(ctx.frame->directionalLightConstantBuffer, &dirLightConstants, sizeof(Lights::DirectionalLightConstants));
    }

    void D3D12ForwardLitGeometryRenderPass::UpdatePointLights(const FrameContext& ctx, const FramePacket& scene)
    {
        Lights::PointLightConstants metadata = {};
        uint32_t count = 0;

        if (!scene.pointLights.empty())
        {
            count = static_cast<uint32_t>(std::min<size_t>(scene.pointLights.size(), ctx.frame->pointLightStructuredBuffer.elementCount));
            Lights::GpuPointLight* dst = reinterpret_cast<Lights::GpuPointLight*>(ctx.frame->pointLightStructuredBuffer.mappedData);

            for (uint32_t i = 0; i < count; ++i)
            {
                const Lights::Point& light = (scene.pointLights)[i];

                dst[i].positionAndRadius = {
                    light.position.x,
                    light.position.y,
                    light.position.z,
                    light.radius
                };

                dst[i].colorAndIntensity = {
                    light.color.x,
                    light.color.y,
                    light.color.z,
                    light.intensity
                };
            }
        }

        metadata.pointLightCount = count;
        m_bufferFactory->UpdateConstantBuffer(ctx.frame->pointLightConstantBuffer, &metadata, sizeof(Lights::PointLightConstants));
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
        D3D12_GPU_VIRTUAL_ADDRESS materialCbAddress = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->materialConstantArena, &material, sizeof(GpuMaterial));
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), materialCbAddress);
        const uint32_t instanceCount = end - begin;
        const UINT64 instanceBufferSize = static_cast<UINT64>(instanceCount) * sizeof(InstanceData);
        GpuUploadBufferAllocation instanceAllocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->gpuUploadBufferArena, instanceBufferSize, InstanceDataAlignment);
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