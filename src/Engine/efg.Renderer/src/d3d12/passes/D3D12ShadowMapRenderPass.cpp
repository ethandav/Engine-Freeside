#include "..\..\..\include\d3d12\passes\ShadowMap\D3D12ShadowMapRenderPass.h"
#include "..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\..\..\include\d3d12\libraries\D3D12MeshLibrary.h"
#include "..\..\..\include\d3d12\factories\D3D12TextrureFactory.h"
#include "..\..\..\include\d3d12\factories\D3D12BufferFactory.h"
#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\include\d3d12\types\D3D12DrawTypes.h"

#include <algorithm>

namespace efg::d3d12
{
    void D3D12ShadowMapRenderPass::Initialize(D3D12GraphicsPipelineLibary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12TextureFactory* textureFactory, D3D12BufferFactory* bufferFactory)
    {
        m_pipelineLibrary = pipelineLib;
        m_descriptorContext = descriptorCtx;
        m_meshLibrary = meshLibrary;
        m_textureFactory = textureFactory;
        m_bufferFactory = bufferFactory;

        m_shadowMap = m_textureFactory->CreateDepthBuffer(2048, 2048, true);
    }

    ShadowMapFrameData D3D12ShadowMapRenderPass::Execute(const FrameContext& ctx, const FramePacket& scene)
    {
        ShadowMapFrameData output = {};
        ShadowMapPassResources resources = {};

        m_viewport.TopLeftX = 0.0f;
        m_viewport.TopLeftY = 0.0f;
        m_viewport.Width = static_cast<float>(2048);
        m_viewport.Height = static_cast<float>(2048);
        m_viewport.MinDepth = 0.0f;
        m_viewport.MaxDepth = 1.0f;
        m_scissorRect.left = 0;
        m_scissorRect.top = 0;
        m_scissorRect.right = static_cast<LONG>(2048);
        m_scissorRect.bottom = static_cast<LONG>(2048);

        ctx.commandContext->SetViewportAndScissor(m_viewport, m_scissorRect);
        ctx.commandContext->SetRenderTarget(0, nullptr, &m_shadowMap.dsv);
        ctx.commandContext->ClearDepthStencil(m_shadowMap.dsv, 1.0f, 0);

        UploadPassResources(ctx, scene, resources, output);
        BindPassResources(ctx, resources);
        DrawAllRenderObjects(ctx, scene);

        output.shadowMap = &m_shadowMap;

        ctx.commandContext->QueueBarrierTransition(m_shadowMap.resource.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        return output;
    }

    void D3D12ShadowMapRenderPass::UploadPassResources(const FrameContext& ctx, const FramePacket& scene, ShadowMapPassResources& resources, ShadowMapFrameData& output)
    {
        UploadFrameConstants(ctx, scene, resources, output);
    }

    void D3D12ShadowMapRenderPass::BindPassResources(const FrameContext& ctx, ShadowMapPassResources& resources)
    {
        ctx.commandContext->BindPipeline(m_pipelineLibrary->Get(PipelineId::ShadowMap));
        ctx.commandContext->SetGraphicsRootConstantBufferView(0, resources.lightViewCB);
        ID3D12DescriptorHeap* heaps[] = { m_descriptorContext->GetCBVSRVUAVHeap() };
        ctx.commandContext->SetDescriptorHeaps(_countof(heaps), heaps);
    }

    void D3D12ShadowMapRenderPass::UploadFrameConstants(const FrameContext& ctx, const FramePacket& scene, ShadowMapPassResources& resources, ShadowMapFrameData& output)
    {
        LightViewConstants lightViewConstants = BuildLightViewConstants(scene.directionalLight);
        output.lightViewProjection = lightViewConstants.viewProjection;
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

            ctx.commandContext->SetGraphicsRootShaderResourceView(1, instanceAllocation.gpu);
            ctx.commandContext->DrawMeshInstanced(m_meshLibrary->Get(batch.mesh), batch.instanceCount);
        }
    }
}