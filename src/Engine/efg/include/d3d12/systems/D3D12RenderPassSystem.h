#pragma once

#include "..\..\render\types\FramePacket.h"
#include "..\frame\D3D12PassContext.h"
#include "..\types\D3D12RenderDeviceServices.h"
#include "..\types\D3D12RenderSceneResources.h"
#include "D3D12ShadowSystem.h"
#include "..\passes\D3D12RenderPasses.h"

namespace efg::d3d12
{
    class D3D12DeviceSystem;
    class D3D12ResourceSystem;
    class D3D12PipelineSystem;
    class D3D12FrameSystem;
    struct RenderTargets;

    class D3D12RenderPassSystem
    {
    public:
        void Initialize(D3D12DeviceSystem* device, D3D12ResourceSystem* resources, D3D12PipelineSystem* pipeline, D3D12FrameSystem* frame, RenderTargets* renderTargets);
        void Execute(const FramePacket& scene, FrameContext& frameCtx, RenderQueue& renderQueue);

    private:
        D3D12PassContext BuildPassContext(FrameContext& frameCtx, RenderQueue& renderQueue);

    private:
        D3D12DeviceSystem* m_device = nullptr;
        D3D12ResourceSystem* m_resources = nullptr;
        D3D12PipelineSystem* m_pipeline = nullptr;
        D3D12FrameSystem* m_frame = nullptr;
        RenderTargets* m_renderTargets = nullptr;

        D3D12RenderDeviceServices m_renderServices = {};
        D3D12RenderResourceLibraries m_renderResources = {};

        D3D12ShadowSystem m_shadowSystem;
        D3D12ShadowMapRenderPass m_shadowMapRenderPass;
        D3D12SkyboxRenderPass m_skyboxRenderPass;
        D3D12ForwardLitGeometryRenderPass m_forwardLitGeometryRenderPass;
        D3D12TonemapRenderPass m_tonemapRenderPass;
    };
}