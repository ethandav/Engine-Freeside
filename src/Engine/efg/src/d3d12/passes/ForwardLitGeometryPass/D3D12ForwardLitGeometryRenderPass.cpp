#include "..\..\..\..\include\d3d12\passes\ForwardLitGeometry\D3D12ForwardLitGeometryRenderPass.h"
#include "..\..\..\..\include\d3d12\core\D3D12DeviceContext.h"
#include "..\..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\..\include\d3d12\types\D3D12DrawTypes.h"
#include "..\..\..\..\include\d3d12\resources\D3D12GpuTexture.h"

#include <algorithm>

namespace efg::d3d12
{
    struct ShadowBinding
    {
        int32_t shadowDataIndex = -1;
        int32_t shadowTextureDescriptorIndex = -1;
    };

    static ShadowBinding FindDirectionalShadowBinding(const ShadowMapFrameData& shadowMapFrameData, uint32_t lightIndex)
    {
        for (int32_t i = 0; i < shadowMapFrameData.directionalShadows.size(); ++i)
        {
            const DirectionalShadowEntry& entry = shadowMapFrameData.directionalShadows[i];

            if (entry.lightIndex == lightIndex)
            {
                return ShadowBinding
                {
                    i,
                    entry.shadowMap->bindlessSrvIndex
                };
            }
        }

        return {};
    }

    static ShadowBinding FindPointShadowBinding(const ShadowMapFrameData& shadowMapFrameData, uint32_t lightIndex)
    {
        for (int32_t i = 0; i < shadowMapFrameData.pointShadows.size(); ++i)
        {
            const PointShadowEntry& entry = shadowMapFrameData.pointShadows[i];

            if (entry.lightIndex == lightIndex)
            {
                return ShadowBinding
                {
                    i,
                    entry.shadowCube->bindlessSrvIndex
                };
            }
        }

        return {};
    }

    void D3D12ForwardLitGeometryRenderPass::Execute(D3D12PassContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData)
    {
        ForwardLitPassResources resources = {};

        UploadPassResources(ctx, scene, shadowMapFrameData, resources);
        BindPassResources(ctx, resources);
        DrawAllRenderObjects(ctx, scene);
    }

    void D3D12ForwardLitGeometryRenderPass::UploadPassResources(D3D12PassContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources)
    {
        UploadDirectionalLights(ctx, scene, shadowMapFrameData, resources);
        UploadPointLights(ctx, scene, shadowMapFrameData, resources);
        UploadShadowResources(ctx, shadowMapFrameData, resources);
        UploadFrameConstants(ctx, scene, resources);
    }

    void D3D12ForwardLitGeometryRenderPass::BindPassResources(D3D12PassContext& ctx, ForwardLitPassResources& resources)
    {
        ID3D12DescriptorHeap* heaps[] =
        {
            ctx.services->descriptors->GetCBVSRVUAVHeap()
        };
        ctx.frameContext->commandContext->SetDescriptorHeaps(_countof(heaps), heaps);
        ctx.frameContext->commandContext->BindPipeline(ctx.services->pipelines->GetPipelineById(PipelineId::ForwardLitGeometry));

        ctx.frameContext->commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), resources.cameraCB);
        ctx.frameContext->commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLightMetadata), resources.directionalLightConstantsCB);
        ctx.frameContext->commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::PointLightMetadata), resources.pointLightConstantsCB);
        ctx.frameContext->commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::ShadowMetadata), resources.shadowMetadataCB);
        ctx.frameContext->commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::PointLightsSrv), resources.pointLightsSRV);
        ctx.frameContext->commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLightsSrv), resources.directionalLightsSRV);
        ctx.frameContext->commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::DirectionalShadowDataSrv), resources.directionalShadowDataSRV);
        ctx.frameContext->commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::PointShadowDataSrv), resources.pointShadowDataSRV);
    }

    void D3D12ForwardLitGeometryRenderPass::UploadFrameConstants(D3D12PassContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources)
    {
        CameraConstants cameraConstants = BuildCameraConstants(scene.camera);
        resources.cameraCB = ctx.services->buffers->CopyToConstantBufferArena(ctx.frameContext->frameResource->constantBufferArena, &cameraConstants, sizeof(CameraConstants));
        resources.directionalLightConstantsCB = ctx.services->buffers->CopyToConstantBufferArena(ctx.frameContext->frameResource->constantBufferArena, &resources.directionalLightConstants, sizeof(DirectionalLightConstants));
        resources.pointLightConstantsCB = ctx.services->buffers->CopyToConstantBufferArena(ctx.frameContext->frameResource->constantBufferArena, &resources.pointLightConstants, sizeof(PointLightConstants));
    }

    CameraConstants D3D12ForwardLitGeometryRenderPass::BuildCameraConstants(const Freeside::Camera& camera)
    {
        CameraConstants cameraConstants = {};
        cameraConstants.viewProjection = Freeside::Math::Transpose(camera.GetViewProjectionMatrix());
        Freeside::Math::Vec3 camPosition = camera.GetPosition();
        cameraConstants.viewPosition = Freeside::Math::Vec4(camPosition.x, camPosition.y, camPosition.z, 0.0f);

        return cameraConstants;
    }

    void D3D12ForwardLitGeometryRenderPass::DrawAllRenderObjects(D3D12PassContext& ctx, const FramePacket& scene)
    {
        for (const auto& batch : ctx.renderQueue->batches)
        {
            const Freeside::RenderObject& first = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex]];
            const Material& material = batch.material.IsValid() ? ctx.libraries->materials->GetMaterialByHandle(batch.material) : ctx.libraries->materials->GetDefaultMaterial();
            const GpuTexture2D baseColorTexture = material.baseColorTexture.IsValid() ? ctx.libraries->textures->GetTexture2DByHandle(material.baseColorTexture) : ctx.libraries->textures->GetDefaultMaterialTexture();
            const GpuTexture2D normalTexture = material.normalTexture.IsValid() ? ctx.libraries->textures->GetTexture2DByHandle(material.normalTexture) : ctx.libraries->textures->GetDefaultNormalTexture();
            const GpuTexture2D heightTexture = material.heightTexture.IsValid() ? ctx.libraries->textures->GetTexture2DByHandle(material.heightTexture) : ctx.libraries->textures->GetDefaultHeightTexture();

            D3D12_GPU_VIRTUAL_ADDRESS materialCbAddress = ctx.services->buffers->CopyToConstantBufferArena(ctx.frameContext->frameResource->constantBufferArena, &material.constants, sizeof(MaterialConstants));
            ctx.frameContext->commandContext->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), materialCbAddress);

            GpuDescriptorTable textureTable = ctx.services->descriptors->AllocateShaderVisibleTableFromFrameArena(ctx.frameContext->frameResource->descriptorArena, 6);
            ctx.frameContext->graphicsContext->GetDevice()->CopyDescriptorsSimple(1, textureTable.cpuStart, baseColorTexture.cpuSrv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            D3D12_CPU_DESCRIPTOR_HANDLE normalDst = textureTable.cpuStart;
            normalDst.ptr += ctx.services->descriptors->GetCBVSRVUAVDescriptorSize();
            ctx.frameContext->graphicsContext->GetDevice()->CopyDescriptorsSimple(1, normalDst, normalTexture.cpuSrv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


            if ((material.constants.materialFlags & Freeside::MaterialFlags::MaterialFlag_HasMetallicRoughnessTexture) != 0)
            {
                const GpuTexture2D metallicTexture = ctx.libraries->textures->GetTexture2DByHandle(material.metallicRoughnessTexture);
                D3D12_CPU_DESCRIPTOR_HANDLE metalDst = textureTable.cpuStart;
                metalDst.ptr += ctx.services->descriptors->GetCBVSRVUAVDescriptorSize() * 2;
                ctx.frameContext->graphicsContext->GetDevice()->CopyDescriptorsSimple(1, metalDst, metallicTexture.cpuSrv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }

            if ((material.constants.materialFlags & Freeside::MaterialFlags::MaterialFlag_HasOcclusionTexture) != 0)
            {
                const GpuTexture2D mocclusionTexture = ctx.libraries->textures->GetTexture2DByHandle(material.occlusionTexture);
                D3D12_CPU_DESCRIPTOR_HANDLE occDst = textureTable.cpuStart;
                occDst.ptr += ctx.services->descriptors->GetCBVSRVUAVDescriptorSize() * 3;
                ctx.frameContext->graphicsContext->GetDevice()->CopyDescriptorsSimple(1, occDst, mocclusionTexture.cpuSrv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }

            if ((material.constants.materialFlags & Freeside::MaterialFlags::MaterialFlag_HasEmissiveTexture) != 0)
            {
                const GpuTexture2D emissiveTexture = ctx.libraries->textures->GetTexture2DByHandle(material.emissiveTexture);
                D3D12_CPU_DESCRIPTOR_HANDLE emmDst = textureTable.cpuStart;
                emmDst.ptr += ctx.services->descriptors->GetCBVSRVUAVDescriptorSize() * 4;
                ctx.frameContext->graphicsContext->GetDevice()->CopyDescriptorsSimple(1, emmDst, emissiveTexture.cpuSrv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }

            if ((material.constants.materialFlags & Freeside::MaterialFlags::MaterialFlag_HasHeightTexture) != 0)
            {
                D3D12_CPU_DESCRIPTOR_HANDLE heightDst = textureTable.cpuStart;
                heightDst.ptr += ctx.services->descriptors->GetCBVSRVUAVDescriptorSize() * 5;
                ctx.frameContext->graphicsContext->GetDevice()->CopyDescriptorsSimple(1, heightDst, heightTexture.cpuSrv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }

            ctx.frameContext->commandContext->SetGraphicsRootDescriptorTable(7, textureTable.gpuStart);

            const UINT64 instanceBufferSize = static_cast<UINT64>(batch.instanceCount) * sizeof(InstanceData);
            GpuUploadBufferAllocation instanceAllocation = ctx.services->buffers->AllocateUploadBufferArena(ctx.frameContext->frameResource->uploadBufferArena, instanceBufferSize, InstanceDataAlignment);
            InstanceData* instances = reinterpret_cast<InstanceData*>(instanceAllocation.cpu);
            for (uint32_t i = 0; i < batch.instanceCount; ++i)
            {
                const Freeside::RenderObject& object = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex + i]];
                instances[i] = {};
                instances[i].world = Freeside::Math::Transpose(object.world);
            }

            ctx.frameContext->commandContext->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::InstanceData), instanceAllocation.gpu);
            ctx.frameContext->commandContext->DrawMeshInstanced(ctx.libraries->meshes->Get(batch.mesh), batch.instanceCount);
        }
    }

    void D3D12ForwardLitGeometryRenderPass::UploadPointLights(D3D12PassContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources)
    {
        uint32_t count = 0;

        if (!scene.pointLights.empty())
        {
            count = static_cast<uint32_t>(scene.pointLights.size());

            GpuUploadBufferAllocation allocation = ctx.services->buffers->AllocateUploadBufferArena(ctx.frameContext->frameResource->uploadBufferArena, count * sizeof(GpuPointLight), StructuredBufferAlignment);
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

                ShadowBinding shadow = FindPointShadowBinding(shadowMapFrameData, i);
                instances[i].shadowDataIndex = shadow.shadowDataIndex;
                instances[i].shadowTextureDescriptorIndex = shadow.shadowTextureDescriptorIndex;
            }
        }

        resources.pointLightConstants.pointLightCount = count;
    }

    void D3D12ForwardLitGeometryRenderPass::UploadDirectionalLights(D3D12PassContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources)
    {
        uint32_t count = 0;

        if (!scene.directionalLights.empty())
        {
            count = static_cast<uint32_t>(scene.directionalLights.size());
            GpuUploadBufferAllocation allocation = ctx.services->buffers->AllocateUploadBufferArena(ctx.frameContext->frameResource->uploadBufferArena, count * sizeof(GpuDirectionalLight), StructuredBufferAlignment);
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

                ShadowBinding shadow = FindDirectionalShadowBinding(shadowMapFrameData, i);
                instances[i].shadowDataIndex = shadow.shadowDataIndex;
                instances[i].shadowTextureDescriptorIndex = shadow.shadowTextureDescriptorIndex;
            }
        }

        resources.directionalLightConstants.directionalLightCount = count;
    }

    void D3D12ForwardLitGeometryRenderPass::UploadShadowResources(D3D12PassContext& ctx, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources)
    {
        const uint32_t directionalShadowCount = std::min<uint32_t>(static_cast<uint32_t>(shadowMapFrameData.directionalShadows.size()), MaxDirectionalShadows);
        const uint32_t pointShadowCount = std::min<uint32_t>(static_cast<uint32_t>(shadowMapFrameData.pointShadows.size()), MaxPointShadows);

        resources.shadowMetadata.directionalShadowCount = directionalShadowCount;
        resources.shadowMetadata.pointShadowCount = pointShadowCount;
        resources.shadowMetadata.shadowBias = 0.001f;
        resources.shadowMetadata.shadowStrength = 1.0f;
        resources.shadowMetadataCB = ctx.services->buffers->CopyToConstantBufferArena(ctx.frameContext->frameResource->constantBufferArena, &resources.shadowMetadata, sizeof(ShadowMetadataConstants));

        if (directionalShadowCount > 0)
        {
            GpuUploadBufferAllocation allocation = ctx.services->buffers->AllocateUploadBufferArena(ctx.frameContext->frameResource->uploadBufferArena, directionalShadowCount * sizeof(GpuDirectionalShadowData), StructuredBufferAlignment);
            GpuDirectionalShadowData* shadowData = reinterpret_cast<GpuDirectionalShadowData*>(allocation.cpu);
            resources.directionalShadowDataSRV = allocation.gpu;

            for (uint32_t i = 0; i < directionalShadowCount; ++i)
            {
                shadowData[i].lightViewProjection = shadowMapFrameData.directionalShadows[i].lightViewProjection;
            }
        }

        if (pointShadowCount > 0)
        {
            GpuUploadBufferAllocation allocation = ctx.services->buffers->AllocateUploadBufferArena(ctx.frameContext->frameResource->uploadBufferArena, pointShadowCount * sizeof(GpuPointShadowData), StructuredBufferAlignment);
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
    }
}