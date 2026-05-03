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
    D3D12_THROW_IF_FAILED(m_commandAllocator->Reset());
    D3D12_THROW_IF_FAILED(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);
    ResourceBarrierTransition(
        m_commandList.Get(),
        m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    Clear();
}

void Renderer::Clear()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
    const float clearColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void Renderer::EndFrame()
{
    ResourceBarrierTransition(
        m_commandList.Get(),
        m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    D3D12_THROW_IF_FAILED(m_commandList->Close());

    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    D3D12_THROW_IF_FAILED(m_swapChain->Present(1, 0));

    WaitForGPU();
}

void Renderer::CreateFence()
{
    D3D12_THROW_IF_FAILED(m_graphicsContext.GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceValue = 1;

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
        D3D12_THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
    }

    WaitForGPU();
}

void Renderer::WaitForGPU()
{
    const UINT64 fence = m_fenceValue;
    D3D12_THROW_IF_FAILED(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fence)
    {
        D3D12_THROW_IF_FAILED(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}