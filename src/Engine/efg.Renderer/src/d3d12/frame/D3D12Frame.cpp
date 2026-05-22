#include "..\..\..\include\d3d12\core\D3D12RendererBackend.h"

namespace efg::d3d12
{
    FrameContext D3D12RendererBackend::BeginFrame()
    {
        uint64_t frameIndex = m_swapChain.GetFrameIndex();
        FrameResource& frame = m_frameResources[frameIndex];
        ID3D12CommandAllocator* allocator = frame.commandAllocator.Get();

        m_directFence.WaitForCPU(frame.fenceValue);
        m_commandContext.BeginRecording(allocator);

        FrameContext ctx = {};
        ctx.frameIndex = frameIndex;
        ctx.frame = &frame;
        ctx.commandContext = &m_commandContext;
        ctx.graphicsContext = &m_graphicsContext;
        ctx.backBuffer = m_swapChain.GetCurrentBackBuffer();
        ctx.backBufferHandle = m_swapChain.GetCurrentBackBufferHandle();
        ctx.renderQueue = &m_renderQueue;

        frame.ResetTransientAllocators();

        return ctx;
    }

    void D3D12RendererBackend::EndFrame(const FrameContext& ctx)
    {
        ID3D12CommandQueue* queue = m_commandContext.GetDirectCommandQueue();
        m_commandContext.ResourceBarrierTransition(m_swapChain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandContext.EndRecording();
        m_commandContext.ExecuteDirect();
        ctx.frame->fenceValue = m_directFence.Signal(queue);
        m_swapChain.Present();
    }

    void D3D12RendererBackend::RecordBackBufferSetup(const FrameContext& ctx)
    {
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        m_commandContext.SetViewportAndScissor(m_viewport, m_scissorRect);
        m_commandContext.ResourceBarrierTransition(ctx.backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandContext.SetRenderTarget(1, &ctx.backBufferHandle, &m_renderTargets.sceneDepth.dsv);
        m_commandContext.ClearRenderTarget(ctx.backBufferHandle, clearColor);
        m_commandContext.ClearDepthStencil(m_renderTargets.sceneDepth.dsv, 1.0f, 0);
    }
}