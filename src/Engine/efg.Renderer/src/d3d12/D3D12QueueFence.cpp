#include "..\..\include\d3d12\D3D12QueueFence.h"
#include "..\..\include\d3d12\D3D12Error.h"

void D3D12QueueFence::Initialize(D3D12Context* graphicsContext)
{
	m_graphicsContext = graphicsContext;
}

void D3D12QueueFence::CreateFence(UINT64 fenceValue)
{
    m_fenceValue = fenceValue;
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
        D3D12_THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
    }
    m_nextFenceValue = m_fenceValue + 1;
}

UINT64 D3D12QueueFence::Signal(ID3D12CommandQueue* queue)
{
    const UINT64 value = m_nextFenceValue++;

    D3D12_THROW_IF_FAILED(queue->Signal(m_fence.Get(), value));

    return value;
}

void D3D12QueueFence::WaitForCPU(UINT64 value)
{
    if (m_fence->GetCompletedValue() < value)
    {
        D3D12_THROW_IF_FAILED(m_fence->SetEventOnCompletion(value, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void D3D12QueueFence::WaitForQueue(ID3D12CommandQueue* queue, UINT64 value)
{
    D3D12_THROW_IF_FAILED(queue->Wait(m_fence.Get(), value));
}

UINT64 D3D12QueueFence::GetCompletedValue() const
{
    return m_fence->GetCompletedValue();
}

void D3D12QueueFence::WaitForGPU(ID3D12CommandQueue* queue)
{
    const UINT64 value = Signal(queue);
    WaitForCPU(value);
}