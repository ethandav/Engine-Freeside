#include "..\..\..\..\include\d3d12\passes\ShadowMap\D3D12ShadowMapRenderPass.h"
#include "..\..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\..\..\..\include\d3d12\libraries\D3D12MeshLibrary.h"
#include "..\..\..\..\include\d3d12\factories\D3D12TextrureFactory.h"
#include "..\..\..\..\include\d3d12\factories\D3D12BufferFactory.h"
#include "..\..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"
#include "..\..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\..\include\d3d12\types\D3D12DrawTypes.h"
#include "..\..\..\..\include\d3d12\core\D3D12Pix.h"
#include "..\..\..\..\include\d3d12\frame\D3D12PassContext.h"

#include <algorithm>

namespace efg::d3d12
{
    void D3D12ShadowMapRenderPass::Execute(D3D12PassContext& ctx, const FramePacket& scene, ShadowMapFrameData& shadowMapFrameData)
    {
        for (const auto& dirShadowMap : shadowMapFrameData.directionalShadows)
        {
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
            PIXBeginEvent(ctx.frameContext->commandContext->GetDirectCommandList(), PixColors::ShadowMapPass, L"Write Directional Light Depth Buffer %u", dirShadowMap.lightIndex + 1);
            ctx.frameContext->commandContext->SetViewportAndScissor(m_viewport, m_scissorRect);
            ctx.frameContext->commandContext->SetRenderTarget(0, nullptr, &dirShadowMap.shadowMap->dsv);
            ctx.frameContext->commandContext->ClearDepthStencil(dirShadowMap.shadowMap->dsv, 1.0f, 0);

            LightViewConstants constants = {};
            constants.viewProjection = dirShadowMap.lightViewProjection;
            resources.lightViewCB = ctx.services->buffers->CopyToConstantBufferArena(ctx.frameContext->frameResource->constantBufferArena, &constants, sizeof(LightViewConstants));
            BindPassResources(ctx, resources);
            DrawAllRenderObjects(ctx, scene);
            PIXEndEvent(ctx.frameContext->commandContext->GetDirectCommandList());
        }

        for (const auto& pointShadow : shadowMapFrameData.pointShadows)
        {
            GpuTextureCube* shadowCube = &pointShadow.shadowCube->depthTest;

            PIXBeginEvent(ctx.frameContext->commandContext->GetDirectCommandList(), PixColors::ShadowMapPass, L"Write Point Light Depth Buffer %u", pointShadow.lightIndex + 1);
            for (uint32_t face = 0; face < 6; ++face)
            {
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

                PIXBeginEvent(ctx.frameContext->commandContext->GetDirectCommandList(), PixColors::ShadowMapPass, L"Write Point Light %u Face %u", pointShadow.lightIndex + 1, face + 1);
                ctx.frameContext->commandContext->SetViewportAndScissor(m_viewport, m_scissorRect);
                ctx.frameContext->commandContext->SetRenderTarget(0, nullptr, &shadowCube->dsv[face]);
                ctx.frameContext->commandContext->ClearDepthStencil(shadowCube->dsv[face], 1.0f, 0);
                LightViewConstants constants = {};
                constants.viewProjection = pointShadow.faceViewProjection[face];
                resources.lightViewCB = ctx.services->buffers->CopyToConstantBufferArena(ctx.frameContext->frameResource->constantBufferArena, &constants, sizeof(LightViewConstants));
                BindPassResources(ctx, resources);
                DrawAllRenderObjects(ctx, scene);
                PIXEndEvent(ctx.frameContext->commandContext->GetDirectCommandList());
            }
            PIXEndEvent(ctx.frameContext->commandContext->GetDirectCommandList());
        }
    }

    void D3D12ShadowMapRenderPass::BindPassResources(D3D12PassContext& ctx, ShadowMapPassResources& resources)
    {
        ctx.frameContext->commandContext->BindPipeline(ctx.services->pipelines->GetPipelineById(PipelineId::ShadowMap));
        ctx.frameContext->commandContext->SetGraphicsRootConstantBufferView(0, resources.lightViewCB);
    }

    void D3D12ShadowMapRenderPass::DrawAllRenderObjects(D3D12PassContext& ctx, const FramePacket& scene)
    {
        for (const auto& batch : ctx.renderQueue->batches)
        {
            const Freeside::RenderObject& first = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex]];
            const UINT64 instanceBufferSize = static_cast<UINT64>(batch.instanceCount) * sizeof(InstanceData);
            GpuUploadBufferAllocation instanceAllocation = ctx.services->buffers->AllocateUploadBufferArena(ctx.frameContext->frameResource->uploadBufferArena, instanceBufferSize, InstanceDataAlignment);
            InstanceData* instances = reinterpret_cast<InstanceData*>(instanceAllocation.cpu);
            for (uint32_t i = 0; i < batch.instanceCount; ++i)
            {
                const Freeside::RenderObject& object = scene.renderObjects[ctx.renderQueue->sortedIndices[batch.firstSortedIndex + i]];
                instances[i] = {};
                instances[i].world = Freeside::Math::Transpose(object.world);
            }

            ctx.frameContext->commandContext->SetGraphicsRootShaderResourceView(1, instanceAllocation.gpu);
            ctx.frameContext->commandContext->DrawMeshInstanced(ctx.libraries->meshes->Get(batch.mesh), batch.instanceCount);
        }
    }
}