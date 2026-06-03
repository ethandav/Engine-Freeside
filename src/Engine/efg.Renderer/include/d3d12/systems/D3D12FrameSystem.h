#pragma once

#include "..\frame\D3D12FrameContext.h"
#include "..\frame\D3D12RenderTargets.h"
#include "..\frame\D3D12FrameResouce.h"
#include "..\frame\D3D12FrameConfig.h"

#include <array>
#include <d3d12.h>

namespace efg::d3d12
{
    class D3D12DeviceSystem;
    class D3D12ResourceSystem;

    class D3D12FrameSystem
    {
    public:
        void Initialize(D3D12DeviceSystem* device, D3D12ResourceSystem* resources, uint32_t width, uint32_t height);
        void Shutdown();
        FrameContext BeginFrame(RenderQueue* renderQueue);
        void EndFrame(const FrameContext& ctx);
        void RecordBackBufferSetup(const FrameContext& ctx, const RenderTargets& renderTargets);
    private:
        void CreateViewportAndScissor(uint32_t width, uint32_t height);
        void CreateFrameResources();
        void DestroyFrameResources();

        D3D12DeviceSystem* m_device = nullptr;
        D3D12ResourceSystem* m_resources = nullptr;
        D3D12_VIEWPORT m_viewport = {};
        D3D12_RECT m_scissorRect = {};
        std::array<FrameResource, NumFramesInFlight> m_frameResources = {};
    };
}