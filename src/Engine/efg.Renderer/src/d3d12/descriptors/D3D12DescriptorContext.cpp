#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"

namespace efg::d3d12
{
    void D3D12DescriptorContext::Initialize(ID3D12Device* device)
    {
        m_device = device;
    }

    void D3D12DescriptorContext::CreateAllHeaps()
    {
        CreateRTVDescriptorHeap();
        CreateCBVSRVUAVDescriptorHeap();
        CreateDSVDescriptorHeap();
        CreateSamplerDescriptorHeap();
    }

    UINT D3D12DescriptorContext::CreateRTVDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = Count;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        D3D12_THROW_IF_FAILED(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_rtvHeapStart = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        m_rtvCapacity = Count;
        m_rtvUsed = 0;
        return m_rtvDescriptorSize;
    }

    UINT D3D12DescriptorContext::CreateCBVSRVUAVDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC shaderVisibleHeapDesc = {};
        shaderVisibleHeapDesc.NumDescriptors = Count;
        shaderVisibleHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        shaderVisibleHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        D3D12_THROW_IF_FAILED(m_device->CreateDescriptorHeap(&shaderVisibleHeapDesc, IID_PPV_ARGS(&m_shaderVisibleHeap)));
        m_shaderVisibleDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_shaderVisibleHeapCpuStart = m_shaderVisibleHeap->GetCPUDescriptorHandleForHeapStart();
        m_shaderVisibleHeapGpuStart = m_shaderVisibleHeap->GetGPUDescriptorHandleForHeapStart();
        m_shaderVisibleCapacity = Count;
        m_shaderVisibleUsed = 0;
        return m_shaderVisibleDescriptorSize;
    }

    UINT D3D12DescriptorContext::CreateDSVDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = Count;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        D3D12_THROW_IF_FAILED(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
        m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_dsvHeapStart = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
        m_dsvCapacity = Count;
        m_dsvUsed = 0;
        return m_dsvDescriptorSize;
    }

    UINT D3D12DescriptorContext::CreateSamplerDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
        samplerHeapDesc.NumDescriptors = Count;
        samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        D3D12_THROW_IF_FAILED(m_device->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_samplerHeap)));
        m_samplerDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        m_samplerHeapCpuStart = m_samplerHeap->GetCPUDescriptorHandleForHeapStart();
        m_samplerHeapGpuStart = m_samplerHeap->GetGPUDescriptorHandleForHeapStart();
        m_samplerCapacity = Count;
        m_samplerUsed = 0;
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

    DescriptorAllocation D3D12DescriptorContext::AllocateDSV()
    {
        if (m_dsvUsed >= m_dsvCapacity)
        {
            throw std::runtime_error("DSV descriptor heap is full.");
        }

        DescriptorAllocation allocation = {};
        allocation.index = m_dsvUsed;
        allocation.cpu = m_dsvHeapStart;
        allocation.cpu.ptr += static_cast<SIZE_T>(m_dsvUsed) * m_dsvDescriptorSize;
        ++m_dsvUsed;

        return allocation;
    }

    DescriptorAllocation D3D12DescriptorContext::AllocateCBVSRVUAV()
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

    DescriptorAllocation D3D12DescriptorContext::AllocateSampler()
    {
        if (m_samplerUsed >= m_samplerCapacity)
        {
            throw std::runtime_error("Sampler descriptor heap is full.");
        }

        DescriptorAllocation allocation = {};
        allocation.index = m_samplerUsed;
        allocation.cpu = m_samplerHeapCpuStart;
        allocation.cpu.ptr += static_cast<SIZE_T>(m_samplerUsed) * m_samplerDescriptorSize;
        allocation.gpu = m_samplerHeapGpuStart;
        allocation.gpu.ptr += static_cast<UINT64>(m_samplerUsed) * m_samplerDescriptorSize;
        ++m_samplerUsed;

        return allocation;
    }

    ID3D12DescriptorHeap* D3D12DescriptorContext::GetCBVSRVUAVHeap()const
    {
        return m_shaderVisibleHeap.Get();
    }

    ID3D12DescriptorHeap* D3D12DescriptorContext::GetSamplerHeap() const
    {
        return m_samplerHeap.Get();
    }
}