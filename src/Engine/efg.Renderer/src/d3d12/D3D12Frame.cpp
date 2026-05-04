#include "..\..\include\d3d12\D3D12RendererBackend.h"

void D3D12RendererBackend::BeginFrame()
{
    UINT frameIndex = m_swapChain.GetFrameIndex();
    FrameResource& frame = m_frameResources[frameIndex];
    ID3D12CommandAllocator* allocator = frame.commandAllocator.Get();
    ID3D12GraphicsCommandList* list = m_commandContext.GetCommandList();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain.GetCurrentRTV();
    const float clearColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    m_frameSync.WaitForFence(frame.fenceValue);
    m_commandContext.BeginRecording(allocator);
    m_commandContext.SetViewportAndScissor(m_viewport, m_scissorRect);
    m_commandContext.SetRenderTarget(rtvHandle);
    m_commandContext.ResourceBarrierTransition(m_swapChain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandContext.ClearRenderTarget(rtvHandle, clearColor);
}

void D3D12RendererBackend::EndFrame()
{
    ID3D12GraphicsCommandList* list = m_commandContext.GetCommandList();
    ID3D12CommandQueue* queue = m_commandContext.GetCommandQueue();
    FrameResource& frame = m_frameResources[m_swapChain.GetFrameIndex()];
    m_commandContext.ResourceBarrierTransition(m_swapChain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandContext.EndRecording();
    m_commandContext.Execute();
    frame.fenceValue = m_frameSync.Signal(queue);
    m_swapChain.Present();
}

