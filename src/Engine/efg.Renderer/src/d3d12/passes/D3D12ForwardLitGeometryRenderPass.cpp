#include "..\..\..\include\d3d12\passes\ForwardLitGeometry\D3D12ForwardLitGeometryRenderPass.h"
#include "..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12MeshLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12MaterialTextureLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12MaterialLibrary.h"
#include "..\..\..\include\d3d12\factories\D3D12BufferFactory.h"
#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"
#include "..\..\..\include\d3d12\core\D3D12Context.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\include\d3d12\types\D3D12DrawTypes.h"

#include <algorithm>

namespace efg::d3d12
{

    static int32_t FindDirectionalShadowIndex(const ShadowMapFrameData& shadowMapFrameData, uint32_t lightIndex)
    {
        for (uint32_t i = 0; i < shadowMapFrameData.directionalShadows.size(); ++i)
        {
            if (shadowMapFrameData.directionalShadows[i].lightIndex == lightIndex)
            {
                return static_cast<int32_t>(i);
            }
        }

        return -1;
    }

    static int32_t FindPointShadowIndex(const ShadowMapFrameData& shadowMapFrameData, uint32_t lightIndex)
    {
        for (uint32_t i = 0; i < shadowMapFrameData.pointShadows.size(); ++i)
        {
            if (shadowMapFrameData.pointShadows[i].lightIndex == lightIndex)
            {
                return static_cast<int32_t>(i);
            }
        }

        return -1;
    }

    void D3D12ForwardLitGeometryRenderPass::Initialize(D3D12GraphicsPipelineLibary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12MaterialLibrary* materialLibrary, D3D12MaterialTextureLibrary* textureLibrary, D3D12BufferFactory* bufferFactory)
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

        UploadPassResources(ctx, scene, shadowMapFrameData, resources);
        BindPassResources(ctx, resources);
        DrawAllRenderObjects(ctx, scene);
    }

    void D3D12ForwardLitGeometryRenderPass::UploadPassResources(const FrameContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources)
    {
        UploadDirectionalLights(ctx, scene, shadowMapFrameData, resources);
        UploadPointLights(ctx, scene, shadowMapFrameData, resources);
        UploadShadowResources(ctx, shadowMapFrameData, resources);
        UploadFrameConstants(ctx, scene, resources);
    }

    void D3D12ForwardLitGeometryRenderPass::BindPassResources(const FrameContext& ctx, ForwardLitPassResources& resources)
    {
        ctx.commandContext->BindPipeline(m_pipelineLibrary->Get(PipelineId::ForwardLitGeometry));

        ID3D12DescriptorHeap* heaps[] =
        {
            m_descriptorContext->GetCBVSRVUAVHeap()
        };

        ctx.commandContext->SetDescriptorHeaps(_countof(heaps), heaps);
        ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), resources.cameraCB);
        ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLightMetadata), resources.directionalLightConstantsCB);
        ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::PointLightMetadata), resources.pointLightConstantsCB);
        ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::ShadowMetadata), resources.shadowMetadataCB);
        ctx.commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::PointLightsSrv), resources.pointLightsSRV);
        ctx.commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLightsSrv), resources.directionalLightsSRV);
        ctx.commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::DirectionalShadowDataSrv), resources.directionalShadowDataSRV);
        ctx.commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::PointShadowDataSrv), resources.pointShadowDataSRV);
        ctx.commandContext->SetGraphicsRootDescriptorTable(static_cast<UINT>(ForwardLitRootParameter::DirectionalShadowMaps), resources.directionalShadowMapsTable);
        ctx.commandContext->SetGraphicsRootDescriptorTable(static_cast<UINT>(ForwardLitRootParameter::PointShadowCubes), resources.pointShadowCubesTable);
    }

    void D3D12ForwardLitGeometryRenderPass::UploadFrameConstants(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources)
    {
        CameraConstants cameraConstants = BuildCameraConstants(scene.camera);
        resources.cameraCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &cameraConstants, sizeof(CameraConstants));
        resources.directionalLightConstantsCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &resources.directionalLightConstants, sizeof(DirectionalLightConstants));
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

    void D3D12ForwardLitGeometryRenderPass::DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene)
    {
        for (const auto& batch : ctx.renderQueue->batches)
        {
            const Freeside::RenderObject& first = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex]];
            const Material& material = batch.material.IsValid() ? m_materialLibrary->GetMaterialByHandle(batch.material) : m_materialLibrary->GetDefaultMaterial();
            D3D12_GPU_VIRTUAL_ADDRESS materialCbAddress = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &material.constants, sizeof(MaterialConstants));
            ctx.commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), materialCbAddress);

            const GpuTexture2D& baseColorTexture = material.baseColorTexture.IsValid() ? m_textureLibrary->GetTextureByHandle(material.baseColorTexture) : m_textureLibrary->GetDefaultMaterialTexture();
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

    void D3D12ForwardLitGeometryRenderPass::BuildDirectionalShadowMapTable(const FrameContext& ctx, const ShadowMapFrameData& shadowMapFrameData, uint32_t activeShadowCount, ForwardLitPassResources& resources)
    {
        GpuDescriptorTable table = m_descriptorContext->AllocateShaderVisibleTableFromFrameArena(ctx.frame->descriptorArena, MaxDirectionalShadows);
        const uint32_t descriptorSize = m_descriptorContext->GetCBVSRVUAVDescriptorSize();

        for (uint32_t i = 0; i < activeShadowCount; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE dst = table.cpuStart;
            D3D12_CPU_DESCRIPTOR_HANDLE src = {};

            dst.ptr += static_cast<SIZE_T>(i) * descriptorSize;
            src = shadowMapFrameData.directionalShadows[i].shadowMap->cpuSrv;
            ctx.graphicsContext->GetDevice()->CopyDescriptorsSimple(1, dst, src, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        resources.directionalShadowMapsTable = table.gpuStart;
    }

    void D3D12ForwardLitGeometryRenderPass::BuildPointShadowCubeTable(const FrameContext& ctx, const ShadowMapFrameData& shadowMapFrameData, uint32_t activeShadowCount, ForwardLitPassResources& resources)
    {
        GpuDescriptorTable table = m_descriptorContext->AllocateShaderVisibleTableFromFrameArena(ctx.frame->descriptorArena, MaxPointShadows);
        const uint32_t descriptorSize = m_descriptorContext->GetCBVSRVUAVDescriptorSize();

        for (uint32_t i = 0; i < activeShadowCount; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE dst = table.cpuStart;
            D3D12_CPU_DESCRIPTOR_HANDLE src = {};

            dst.ptr += static_cast<SIZE_T>(i) * descriptorSize;
            src = shadowMapFrameData.pointShadows[i].shadowCube->cpuSrv;
            ctx.graphicsContext->GetDevice()->CopyDescriptorsSimple(1, dst, src, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        resources.pointShadowCubesTable = table.gpuStart;
    }

    void D3D12ForwardLitGeometryRenderPass::UploadPointLights(const FrameContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources)
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
                const Freeside::Lights::Point& light = scene.pointLights[i];

                instances[i].positionAndRadius =
                {
                    light.position.x,
                    light.position.y,
                    light.position.z,
                    light.radius
                };

                instances[i].colorAndIntensity =
                {
                    light.color.x,
                    light.color.y,
                    light.color.z,
                    light.intensity
                };

                instances[i].shadowIndex = FindPointShadowIndex(shadowMapFrameData, i);
            }
        }

        resources.pointLightConstants.pointLightCount = count;
    }

    void D3D12ForwardLitGeometryRenderPass::UploadDirectionalLights(const FrameContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources)
    {
        uint32_t count = 0;

        if (!scene.directionalLights.empty())
        {
            count = static_cast<uint32_t>(scene.directionalLights.size());
            GpuUploadBufferAllocation allocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, count * sizeof(GpuDirectionalLight), StructuredBufferAlignment);
            GpuDirectionalLight* instances = reinterpret_cast<GpuDirectionalLight*>(allocation.cpu);
            resources.directionalLightsSRV = allocation.gpu;

            for (uint32_t i = 0; i < count; ++i)
            {
                const Freeside::Lights::Directional& light = scene.directionalLights[i];

                instances[i].directionAndIntensity =
                {
                    light.direction.x,
                    light.direction.y,
                    light.direction.z,
                    light.intensity
                };

                instances[i].colorAndPadding =
                {
                    light.color.x,
                    light.color.y,
                    light.color.z,
                    0.0f
                };

                instances[i].shadowIndex = FindDirectionalShadowIndex(shadowMapFrameData, i);
            }
        }

        resources.directionalLightConstants.directionalLightCount = count;
    }

    void D3D12ForwardLitGeometryRenderPass::UploadShadowResources(const FrameContext& ctx, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources)
    {
        const uint32_t directionalShadowCount = std::min<uint32_t>(static_cast<uint32_t>(shadowMapFrameData.directionalShadows.size()), MaxDirectionalShadows);
        const uint32_t pointShadowCount = std::min<uint32_t>(static_cast<uint32_t>(shadowMapFrameData.pointShadows.size()), MaxPointShadows);

        resources.shadowMetadata.directionalShadowCount = directionalShadowCount;
        resources.shadowMetadata.pointShadowCount = pointShadowCount;
        resources.shadowMetadata.shadowBias = 0.001f;
        resources.shadowMetadata.shadowStrength = 1.0f;
        resources.shadowMetadataCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &resources.shadowMetadata, sizeof(ShadowMetadataConstants));

        if (directionalShadowCount > 0)
        {
            GpuUploadBufferAllocation allocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, directionalShadowCount * sizeof(GpuDirectionalShadowData), StructuredBufferAlignment);
            GpuDirectionalShadowData* shadowData = reinterpret_cast<GpuDirectionalShadowData*>(allocation.cpu);
            resources.directionalShadowDataSRV = allocation.gpu;

            for (uint32_t i = 0; i < directionalShadowCount; ++i)
            {
                shadowData[i].lightViewProjection = shadowMapFrameData.directionalShadows[i].lightViewProjection;
            }
        }
        else
        {
            //resources.directionalShadowDataSRV = m_dummyStructuredBufferGpuVA;
        }

        if (pointShadowCount > 0)
        {
            GpuUploadBufferAllocation allocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, pointShadowCount * sizeof(GpuPointShadowData), StructuredBufferAlignment);
            GpuPointShadowData* shadowData = reinterpret_cast<GpuPointShadowData*>(allocation.cpu);
            resources.pointShadowDataSRV = allocation.gpu;

            for (uint32_t i = 0; i < pointShadowCount; ++i)
            {
                const PointShadowEntry& entry = shadowMapFrameData.pointShadows[i];

                for (uint32_t face = 0; face < 6; ++face)
                {
                    shadowData[i].faceViewProjection[face] = entry.faceViewProjection[face];
                }

                shadowData[i].farPlane = entry.farPlane;
            }
        }
        else
        {
            //resources.pointShadowDataSRV = m_dummyStructuredBufferGpuVA;
        }

        BuildDirectionalShadowMapTable(ctx, shadowMapFrameData, directionalShadowCount, resources);
        BuildPointShadowCubeTable(ctx, shadowMapFrameData, pointShadowCount, resources);
    }
}