#include "..\..\..\include\d3d12\core\D3D12RendererBackend.h"

namespace efg::d3d12
{
    FrameContext D3D12RendererBackend::BeginFrame()
    {
        uint64_t frameIndex = m_device.SwapChain().GetFrameIndex();
        FrameResource& frame = m_frameResources[frameIndex];
        ID3D12CommandAllocator* allocator = frame.commandAllocator.Get();

        m_device.DirectFence().WaitForCPU(frame.fenceValue);
        m_device.DirectCommandContext().BeginRecording(allocator);

        FrameContext ctx = {};
        ctx.frameIndex = frameIndex;
        ctx.frameResource = &frame;
        ctx.commandContext = &m_device.DirectCommandContext();
        ctx.graphicsContext = &m_device.GraphicsContext();
        ctx.backBuffer = m_device.SwapChain().GetCurrentBackBuffer();
        ctx.backBufferHandle = m_device.SwapChain().GetCurrentBackBufferHandle();
        ctx.renderQueue = &m_renderQueue;

        frame.ResetTransientAllocators();

        return ctx;
    }

    void D3D12RendererBackend::EndFrame(const FrameContext& ctx)
    {
        ID3D12CommandQueue* queue = m_device.DirectCommandContext().GetDirectCommandQueue();
        m_device.DirectCommandContext().ResourceBarrierTransition(m_device.SwapChain().GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_device.DirectCommandContext().EndRecording();
        m_device.DirectCommandContext().ExecuteDirect();
        ctx.frameResource->fenceValue = m_device.DirectFence().Signal(queue);
        m_device.SwapChain().Present();
    }

    void D3D12RendererBackend::RecordBackBufferSetup(const FrameContext& ctx)
    {
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        m_device.DirectCommandContext().SetViewportAndScissor(m_viewport, m_scissorRect);
        m_device.DirectCommandContext().ResourceBarrierTransition(ctx.backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_device.DirectCommandContext().SetRenderTarget(1, &ctx.backBufferHandle, &m_renderTargets.sceneDepth.dsv);
        m_device.DirectCommandContext().ClearRenderTarget(ctx.backBufferHandle, clearColor);
        m_device.DirectCommandContext().ClearDepthStencil(m_renderTargets.sceneDepth.dsv, 1.0f, 0);
    }
}