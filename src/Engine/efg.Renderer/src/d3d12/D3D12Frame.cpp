#include "..\..\include\d3d12\D3D12RendererBackend.h"

void D3D12RendererBackend::BeginFrame()
{
    UINT frameIndex = m_swapChain.GetFrameIndex();
    FrameResource& frame = m_frameResources[frameIndex];
    ID3D12CommandAllocator* allocator = frame.commandAllocator.Get();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain.GetCurrentRTV();
    const float clearColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    m_directFence.WaitForCPU(frame.fenceValue);
    m_commandContext.BeginRecording(allocator);
    if (m_uploadContext.queueSize > 0)
    {
        FlushPendingUploads();
    }
    if (m_uploadContext.pendingBatchesSize > 0)
    {
        m_uploadContext.RetireCompletedUploads();
    }
    m_commandContext.SetViewportAndScissor(m_viewport, m_scissorRect);
    m_commandContext.ResourceBarrierTransition(m_swapChain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandContext.SetRenderTarget(rtvHandle);
    m_commandContext.ClearRenderTarget(rtvHandle, clearColor);
}

void D3D12RendererBackend::EndFrame()
{
    ID3D12CommandQueue* queue = m_commandContext.GetDirectCommandQueue();
    FrameResource& frame = m_frameResources[m_swapChain.GetFrameIndex()];
    m_commandContext.ResourceBarrierTransition(m_swapChain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandContext.EndRecording();
    m_commandContext.ExecuteDirect();
    frame.fenceValue = m_directFence.Signal(queue);
    m_swapChain.Present();
}

