#include "..\..\..\include\d3d12\passes\ForwardLitGeometry\D3D12ForwardLitGeometryRenderPass.h"
#include "..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12MeshLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12TextureLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12MaterialLibrary.h"
#include "..\..\..\include\d3d12\factories\D3D12BufferFactory.h"
#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\include\d3d12\types\D3D12DrawTypes.h"

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

    void D3D12ForwardLitGeometryRenderPass::Execute(const FrameContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData)
	{
        ForwardLitPassResources resources = {};
        resources.shadowMapSRV = shadowMapFrameData.shadowMap->gpuSrv;

        Freeside::Camera lightCamera;
        Freeside::Math::Vec3 sceneCenter = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
        Freeside::Math::Vec3 lightDir = Freeside::Math::Normalize(scene.directionalLight.direction);
        float lightDistance = 50.0f;
        Freeside::Math::Vec3 lightPosition = sceneCenter - lightDir * lightDistance;

        float orthoWidth = 40.0f;
        float orthoHeight = 40.0f;
        float nearZ = 0.1f;
        float farZ = 100.0f;
        lightCamera.LookAt(lightPosition, sceneCenter, Freeside::Math::Vec3(0.0f, 1.0f, 0.0f));

        lightCamera.SetOrthographic(orthoWidth, orthoHeight, nearZ, farZ);
        Freeside::Math::Mat4 viewProjection = Freeside::Math::Transpose(lightCamera.GetViewProjectionMatrix());
        ShadowConstants shadowConstants = {};
        shadowConstants.LightViewProjection = viewProjection;
        shadowConstants.ShadowParams = Freeside::Math::Vec4(0.001f, 1.0f, 0.0f, 0.0f);
        resources.shadowCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &shadowConstants, sizeof(ShadowConstants));

        UploadPassResources(ctx, scene, resources);
        BindPassResources(ctx, resources);
        ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Shadow), resources.shadowCB);
        DrawAllRenderObjects(ctx, scene);
	}

    void D3D12ForwardLitGeometryRenderPass::UploadPassResources(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources)
    {
        UploadPointLights(ctx, scene, resources);
        UploadFrameConstants(ctx, scene, resources);
    }

    void D3D12ForwardLitGeometryRenderPass::BindPassResources(const FrameContext& ctx, ForwardLitPassResources& resources)
    {
        ctx.commandContext->BindPipeline(m_pipelineLibrary->Get(PipelineId::ForwardLitGeometry));
        ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), resources.cameraCB);
        ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLight), resources.directionalLightCB);
        ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::PointLightConstants), resources.pointLightConstantsCB);
        ctx.commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::PointLightsSrv), resources.pointLightsSRV);
        ID3D12DescriptorHeap* heaps[] = { m_descriptorContext->GetCBVSRVUAVHeap() };
        ctx.commandContext->SetDescriptorHeaps(_countof(heaps), heaps);
        ctx.commandContext->SetGraphicsRootDescriptorTable(8, resources.shadowMapSRV);
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
            ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), materialCbAddress);

            const GpuTexture2D& baseColorTexture = m_textureLibrary->GetTextureByHandle(material.baseColorTexture);
            ctx.commandContext->SetGraphicsRootDescriptorTable(7, baseColorTexture.gpuSrv);

            const UINT64 instanceBufferSize = static_cast<UINT64>(batch.instanceCount) * sizeof(InstanceData);
            GpuUploadBufferAllocation instanceAllocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, instanceBufferSize, InstanceDataAlignment);
            InstanceData* instances = reinterpret_cast<InstanceData*>(instanceAllocation.cpu);
            for (uint32_t i = 0; i < batch.instanceCount; ++i)
            {
                const Freeside::RenderObject& object = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex + i]];
                instances[i] = {};
                instances[i].world = Freeside::Math::Transpose(object.world);
            }

            ctx.commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::InstanceData), instanceAllocation.gpu);
            ctx.commandContext->DrawMeshInstanced(m_meshLibrary->Get(batch.mesh), batch.instanceCount);
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
}