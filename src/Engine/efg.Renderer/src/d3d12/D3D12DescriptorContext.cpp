#include "..\..\include\d3d12\D3D12DescriptorContext.h"
#include "..\..\include\d3d12\D3D12Error.h"


void D3D12DescriptorContext::Initialize(D3D12Context* graphicsContext)
{
    m_graphicsContext = graphicsContext;
}

void D3D12DescriptorContext::CreateAllHeaps()
{
    CreateRTVDescriptorHeap();
    CreateShaderResourceDescriptorHeap();
    CreateDSVDescriptorHeap();
    CreateSamplerDescriptorHeap();
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

UINT D3D12DescriptorContext::CreateShaderResourceDescriptorHeap(const UINT Count)
{
    D3D12_DESCRIPTOR_HEAP_DESC shaderVisibleHeapDesc = {};
    shaderVisibleHeapDesc.NumDescriptors = Count;
    shaderVisibleHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    shaderVisibleHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateDescriptorHeap(&shaderVisibleHeapDesc, IID_PPV_ARGS(&m_shaderVisibleHeap)));
    m_shaderVisibleDescriptorSize = m_graphicsContext->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_shaderVisibleHeapCpuStart = m_shaderVisibleHeap->GetCPUDescriptorHandleForHeapStart();
    m_shaderVisibleHeapGpuStart = m_shaderVisibleHeap->GetGPUDescriptorHandleForHeapStart();
    m_shaderVisibleCapacity = Count;
    return m_shaderVisibleDescriptorSize;
}

UINT D3D12DescriptorContext::CreateDSVDescriptorHeap(const UINT Count)
{
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = Count;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
    m_dsvDescriptorSize = m_graphicsContext->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_dsvHeapStart = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    m_dsvCapacity = Count;
    return m_dsvDescriptorSize;
}

UINT D3D12DescriptorContext::CreateSamplerDescriptorHeap(const UINT Count)
{
    D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
    samplerHeapDesc.NumDescriptors = Count;
    samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_samplerHeap)));
    m_samplerDescriptorSize = m_graphicsContext->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    m_samplerHeapCpuStart = m_samplerHeap->GetCPUDescriptorHandleForHeapStart();
    m_samplerHeapGpuStart = m_samplerHeap->GetGPUDescriptorHandleForHeapStart();
    m_samplerCapacity = Count;
    return m_samplerDescriptorSize;
}

DescriptorAllocation D3D12DescriptorContext::AllocateRTV()
{
    if (m_rtvUsed >= m_rtvCapacity)
    {
        throw std::runtime_error("RTV descriptor heap is full.");
    }

    DescriptorAllocation allocation = {};
    allocation.index = m_rtvUsed;
    allocation.cpu = m_rtvHeapStart;
    allocation.cpu.ptr += static_cast<SIZE_T>(m_rtvUsed) * m_rtvDescriptorSize;

    ++m_rtvUsed;

    return allocation;
}

DescriptorAllocation D3D12DescriptorContext::CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc)
{
    DescriptorAllocation allocation = AllocateRTV();

    m_graphicsContext->GetDevice()->CreateRenderTargetView(resource, desc, allocation.cpu);

    return allocation;
}

DescriptorAllocation D3D12DescriptorContext::AllocateshaderVisible()
{
    if (m_shaderVisibleUsed >= m_shaderVisibleCapacity)
    {
        throw std::runtime_error("CBV/SRV/UAV heap is full.");
    }

    DescriptorAllocation allocation = {};
    allocation.index = m_shaderVisibleUsed;
    allocation.cpu = m_shaderVisibleHeapCpuStart;
    allocation.cpu.ptr += static_cast<SIZE_T>(m_shaderVisibleUsed) * m_shaderVisibleDescriptorSize;
    allocation.gpu = m_shaderVisibleHeapGpuStart;
    allocation.gpu.ptr += static_cast<UINT64>(m_shaderVisibleUsed) * m_shaderVisibleDescriptorSize;

    ++m_shaderVisibleUsed;

    return allocation;
}

ID3D12DescriptorHeap* D3D12DescriptorContext::GetShaderVisibleHeap()
{
    return m_shaderVisibleHeap.Get();
}