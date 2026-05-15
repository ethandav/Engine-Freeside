#include "..\..\..\include\d3d12\passes\D3D12ForwardLitGeometryRenderPass.h"
#include "..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12MeshLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12TextureLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12MaterialLibrary.h"
#include "..\..\..\include\d3d12\factories\D3D12BufferFactory.h"
#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"

#include <algorithm>

namespace efg::d3d12
{
    void D3D12ForwardLitGeometryRenderPass::Initialize(D3D12GraphicsPipelineLibary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12MaterialLibrary* materialLibrary, D3D12TextureLibrary* textureLibrary, D3D12BufferFactory* bufferFactory)
    {
        m_pipelineLibrary = pipelineLib;
        m_descriptorContext = descriptorCtx;
        m_meshLibrary = meshLibrary;
        m_materialLibrary = materialLibrary;
        m_textureLibrary = textureLibrary;
        m_bufferFactory = bufferFactory;
    }

    void D3D12ForwardLitGeometryRenderPass::Execute(const FrameContext& ctx, const FramePacket& scene)
	{
        ForwardLitPassResources resources = {};
        UploadPassResources(ctx, scene, resources);
        BindPassResources(ctx, resources);
        DrawAllRenderObjects(ctx, scene);
	}

    void D3D12ForwardLitGeometryRenderPass::UploadPassResources(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources)
    {
        UploadPointLights(ctx, scene, resources);
        UploadFrameConstants(ctx, scene, resources);
    }

    void D3D12ForwardLitGeometryRenderPass::BindPassResources(const FrameContext& ctx, ForwardLitPassResources& resources)
    {
        m_pipelineLibrary->BindPipeline(ctx.commandList, PipelineId::ForwardLitGeometry);
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), resources.cameraCB);
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLight), resources.directionalLightCB);
        ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::PointLightConstants), resources.pointLightConstantsCB);
        ctx.commandList->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::PointLightsSrv), resources.pointLightsSRV);
        ID3D12DescriptorHeap* heaps[] = { m_descriptorContext->GetCBVSRVUAVHeap() };
        ctx.commandList->SetDescriptorHeaps(_countof(heaps), heaps);
    }

    void D3D12ForwardLitGeometryRenderPass::UploadFrameConstants(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources)
    {
        CameraConstants cameraConstants = BuildCameraConstants(scene.camera);
        DirectionalLightConstants dirLightConstants = BuildDirectionalLightConstants(scene.directionalLight);
        resources.cameraCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &cameraConstants, sizeof(CameraConstants));
        resources.directionalLightCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &dirLightConstants, sizeof(DirectionalLightConstants));
        resources.pointLightConstantsCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &resources.pointLightConstants, sizeof(PointLightConstants));
    }

    CameraConstants D3D12ForwardLitGeometryRenderPass::BuildCameraConstants(const Freeside::Camera& camera)
    {
        CameraConstants cameraConstants = {};
        cameraConstants.viewProjection = Freeside::Math::Transpose(camera.GetViewProjectionMatrix());
        Freeside::Math::Vec3 camPosition = camera.GetPosition();
        cameraConstants.viewPosition = Freeside::Math::Vec4(camPosition.x, camPosition.y, camPosition.z, 0.0f);

        return cameraConstants;
    }

    DirectionalLightConstants D3D12ForwardLitGeometryRenderPass::BuildDirectionalLightConstants(const Freeside::Lights::Directional& light)
    {
        DirectionalLightConstants constants = {};
        constants.directionAndIntensity = {light.direction.x, light.direction.y, light.direction.z, 1.0f};
        constants.colorAndPadding = {light.color.x, light.color.y, light.color.z, 1.0f};
        return constants;
    }

    void D3D12ForwardLitGeometryRenderPass::DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene)
    {
        for (const auto& batch : ctx.renderQueue->batches)
        {
            const Freeside::RenderObject& first = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex]];
            const Material& material = batch.material.IsValid() ? m_materialLibrary->GetMaterialByHandle(batch.material) : m_materialLibrary->GetDefaultMaterial();
            D3D12_GPU_VIRTUAL_ADDRESS materialCbAddress = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &material.constants, sizeof(MaterialConstants));
            ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), materialCbAddress);

            const GpuTexture2D& baseColorTexture = m_textureLibrary->GetTextureByHandle(material.baseColorTexture);
            ctx.commandList->SetGraphicsRootDescriptorTable(7, baseColorTexture.gpuSrv);

            const UINT64 instanceBufferSize = static_cast<UINT64>(batch.instanceCount) * sizeof(InstanceData);
            GpuUploadBufferAllocation instanceAllocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, instanceBufferSize, InstanceDataAlignment);
            InstanceData* instances = reinterpret_cast<InstanceData*>(instanceAllocation.cpu);
            for (uint32_t i = 0; i < batch.instanceCount; ++i)
            {
                const Freeside::RenderObject& object = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex + i]];
                instances[i] = {};
                instances[i].world = Freeside::Math::Transpose(object.world);
            }

            ctx.commandList->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::InstanceData), instanceAllocation.gpu);
            DrawMeshInstanced(ctx.commandList, batch.mesh, batch.instanceCount);
        }
    }

    void D3D12ForwardLitGeometryRenderPass::UploadPointLights(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources)
    {
        uint32_t count = 0;

        if (!scene.pointLights.empty())
        {
            count = static_cast<uint32_t>(scene.pointLights.size());
            GpuUploadBufferAllocation allocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, count * sizeof(GpuPointLight), StructuredBufferAlignment);
            GpuPointLight* instances = reinterpret_cast<GpuPointLight*>(allocation.cpu);
            resources.pointLightsSRV = allocation.gpu;

            for (uint32_t i = 0; i < count; ++i)
            {
                const Freeside::Lights::Point& light = (scene.pointLights)[i];
                instances[i].positionAndRadius = {light.position.x, light.position.y, light.position.z, light.radius};
                instances[i].colorAndIntensity = {light.color.x, light.color.y, light.color.z, light.intensity};
            }
        }

        resources.pointLightConstants.pointLightCount = count;
    }

    void D3D12ForwardLitGeometryRenderPass::DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, Freeside::MeshHandle handle, uint32_t instanceCount)
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