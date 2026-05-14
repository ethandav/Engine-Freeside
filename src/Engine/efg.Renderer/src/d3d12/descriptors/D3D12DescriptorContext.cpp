#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"

namespace efg::d3d12
{
    static uint32_t AlignConstantBufferSize(uint32_t size)
    {
        return (size + 255) & ~255;
    }

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

    DescriptorAllocation D3D12DescriptorContext::CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc)
    {
        DescriptorAllocation allocation = AllocateRTV();
        m_device->CreateRenderTargetView(resource, desc, allocation.cpu);

        return allocation;
    }

    DescriptorAllocation D3D12DescriptorContext::CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
    {
        DescriptorAllocation allocation = AllocateDSV();
        m_device->CreateDepthStencilView(resource, desc, allocation.cpu);

        return allocation;
    }

    DescriptorAllocation D3D12DescriptorContext::CreateCBV(ID3D12Resource* resource, uint32_t sizeInBytes)
    {
        DescriptorAllocation allocation = AllocateCBVSRVUAV();

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = AlignConstantBufferSize(sizeInBytes);
        m_device->CreateConstantBufferView(&cbvDesc, allocation.cpu);

        return allocation;
    }

    DescriptorAllocation D3D12DescriptorContext::CreateUAV(ID3D12Resource* resource, uint32_t elementCount, uint32_t elementStride, ID3D12Resource* counterResource)
    {
        DescriptorAllocation allocation = AllocateCBVSRVUAV();

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = elementCount;
        uavDesc.Buffer.StructureByteStride = elementStride;
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
        m_device->CreateUnorderedAccessView(resource, counterResource, &uavDesc, allocation.cpu);

        return allocation;
    }

    DescriptorAllocation D3D12DescriptorContext::CreateStructuredBufferSRV(ID3D12Resource* resource, uint32_t elementCount, uint32_t elementStride)
    {
        DescriptorAllocation allocation = AllocateCBVSRVUAV();
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = elementCount;
        srvDesc.Buffer.StructureByteStride = elementStride;
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        m_device->CreateShaderResourceView(resource, &srvDesc, allocation.cpu);

        return allocation;
    }

    DescriptorAllocation D3D12DescriptorContext::CreateTexture2DSRV(ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels)
    {
        DescriptorAllocation allocation = AllocateCBVSRVUAV();
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = mipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.PlaneSlice = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        m_device->CreateShaderResourceView(resource, &srvDesc, allocation.cpu);

        return allocation;
    }

    DescriptorAllocation D3D12DescriptorContext::CreateSampler(const D3D12_SAMPLER_DESC& samplerDesc)
    {
        DescriptorAllocation allocation = AllocateSampler();
        m_device->CreateSampler(&samplerDesc, allocation.cpu);

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