#include "..\..\..\include\d3d12\passes\D3D12ShadowMapRenderPass.h"
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
    void D3D12ShadowMapRenderPass::Initialize(D3D12GraphicsPipelineLibary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12MaterialLibrary* materialLibrary, D3D12TextureLibrary* textureLibrary, D3D12BufferFactory* bufferFactory)
    {
        m_pipelineLibrary = pipelineLib;
        m_descriptorContext = descriptorCtx;
        m_meshLibrary = meshLibrary;
        m_materialLibrary = materialLibrary;
        m_textureLibrary = textureLibrary;
        m_bufferFactory = bufferFactory;
    }

    void D3D12ShadowMapRenderPass::Execute(const FrameContext& ctx, const FramePacket& scene)
    {
        ShadowMapPassResources resources = {};
        UploadPassResources(ctx, scene, resources);
        BindPassResources(ctx, resources);
        DrawAllRenderObjects(ctx, scene);
    }

    void D3D12ShadowMapRenderPass::UploadPassResources(const FrameContext& ctx, const FramePacket& scene, ShadowMapPassResources& resources)
    {
        UploadFrameConstants(ctx, scene, resources);
    }

    void D3D12ShadowMapRenderPass::BindPassResources(const FrameContext& ctx, ShadowMapPassResources& resources)
    {
        m_pipelineLibrary->BindPipeline(ctx.commandList, PipelineId::ShadowMap);
        ctx.commandList->SetGraphicsRootConstantBufferView(0, resources.lightViewCB);
        ID3D12DescriptorHeap* heaps[] = { m_descriptorContext->GetCBVSRVUAVHeap() };
        ctx.commandList->SetDescriptorHeaps(_countof(heaps), heaps);
    }

    void D3D12ShadowMapRenderPass::UploadFrameConstants(const FrameContext& ctx, const FramePacket& scene, ShadowMapPassResources& resources)
    {
        LightViewConstants lightViewConstants = BuildLightViewConstants(scene.directionalLight);
        resources.lightViewCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &lightViewConstants, sizeof(LightViewConstants));
    }

    LightViewConstants D3D12ShadowMapRenderPass::BuildLightViewConstants(const Freeside::Lights::Directional& dirLight)
    {
        LightViewConstants lightViewConstants = {};

        Freeside::Camera lightCamera;
        Freeside::Math::Vec3 sceneCenter = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
        Freeside::Math::Vec3 lightDir = Freeside::Math::Normalize(dirLight.direction);
        float lightDistance = 50.0f;
        Freeside::Math::Vec3 lightPosition = sceneCenter - lightDir * lightDistance;

        float orthoWidth = 40.0f;
        float orthoHeight = 40.0f;
        float nearZ = 0.1f;
        float farZ = 100.0f;
        lightCamera.LookAt(lightPosition, sceneCenter, Freeside::Math::Vec3(0.0f, 1.0f, 0.0f));

        lightCamera.SetOrthographic(orthoWidth, orthoHeight, nearZ, farZ);
        lightViewConstants.viewProjection = Freeside::Math::Transpose(lightCamera.GetViewProjectionMatrix());

        return lightViewConstants;
    }

    void D3D12ShadowMapRenderPass::DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene)
    {
        for (const auto& batch : ctx.renderQueue->batches)
        {
            const Freeside::RenderObject& first = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex]];
            const UINT64 instanceBufferSize = static_cast<UINT64>(batch.instanceCount) * sizeof(InstanceData);
            GpuUploadBufferAllocation instanceAllocation = m_bufferFactory->AllocateUploadBufferArena(ctx.frame->uploadBufferArena, instanceBufferSize, InstanceDataAlignment);
            InstanceData* instances = reinterpret_cast<InstanceData*>(instanceAllocation.cpu);
            for (uint32_t i = 0; i < batch.instanceCount; ++i)
            {
                const Freeside::RenderObject& object = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex + i]];
                instances[i] = {};
                instances[i].world = Freeside::Math::Transpose(object.world);
            }

            ctx.commandList->SetGraphicsRootShaderResourceView(1, instanceAllocation.gpu);
            DrawMeshInstanced(ctx.commandList, batch.mesh, batch.instanceCount);
        }
    }

    void D3D12ShadowMapRenderPass::DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, Freeside::MeshHandle handle, uint32_t instanceCount)
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