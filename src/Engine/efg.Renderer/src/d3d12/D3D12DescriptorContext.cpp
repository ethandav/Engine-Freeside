#include "..\..\include\d3d12\D3D12DescriptorContext.h"
#include "..\..\include\d3d12\D3D12Error.h"


void D3D12DescriptorContext::Initialize(D3D12Context* graphicsContext)
{
    m_graphicsContext = graphicsContext;
}

void D3D12DescriptorContext::CreateAllHeaps()
{
    CreateRTVDescriptorHeap();
}

UINT D3D12DescriptorContext::CreateRTVDescriptorHeap(const UINT Count)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = Count;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
    m_rtvDescriptorSize = m_graphicsContext->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_rtvHeapStart = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    m_rtvCapacity = Count;
    return m_rtvDescriptorSize;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorContext::AllocateRTV()
{
    if (m_rtvUsed >= m_rtvCapacity)
    {
        throw std::runtime_error("RTV descriptor heap is full.");
    }

    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeapStart;
    handle.ptr += static_cast<SIZE_T>(m_rtvUsed) * m_rtvDescriptorSize;
    ++m_rtvUsed;

    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorContext::CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = AllocateRTV();

    m_graphicsContext->GetDevice()->CreateRenderTargetView(resource, desc, handle);

    return handle;
}