#include <d3dx12.h>

#include "..\..\include\renderer.h"

void ResourceBarrierTransition(ID3D12GraphicsCommandList* commandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        pResource,
        before,
        after
    );
    commandList->ResourceBarrier(1, &barrier);
}

void Renderer::BeginFrame()
{
    UINT frameIndex = m_swapChain.GetFrameIndex();
    FrameResource& frame = m_frameResources[frameIndex];
    ID3D12CommandAllocator* allocator = frame.commandAllocator.Get();
    ID3D12GraphicsCommandList* list = m_commandContext.GetCommandList();
    m_frameSync.WaitForFence(frame.fenceValue);
    D3D12_THROW_IF_FAILED(allocator->Reset());
    D3D12_THROW_IF_FAILED(list->Reset(allocator, nullptr));
    list->RSSetViewports(1, &m_viewport);
    list->RSSetScissorRects(1, &m_scissorRect);
    ResourceBarrierTransition(
        list,
        m_swapChain.GetCurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    Clear();
}

void Renderer::Clear()
{
    ID3D12GraphicsCommandList* list = m_commandContext.GetCommandList();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain.GetCurrentRTV();
    list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
    const float clearColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    list->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void Renderer::EndFrame()
{
    ID3D12GraphicsCommandList* list = m_commandContext.GetCommandList();
    ID3D12CommandQueue* queue = m_commandContext.GetCommandQueue();
    ResourceBarrierTransition(
        list,
        m_swapChain.GetCurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    D3D12_THROW_IF_FAILED(list->Close());

    ID3D12CommandList* ppCommandLists[] = { list };
    queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    m_swapChain.Present();
    FrameResource& frame = m_frameResources[m_swapChain.GetFrameIndex()];
    frame.fenceValue = m_frameSync.Signal(queue);
}

