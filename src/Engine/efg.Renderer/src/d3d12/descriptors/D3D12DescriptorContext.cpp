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
        CreateShaderVisibleCBVSRVUAVDescriptorHeap();
        CreateCPUOnlyCBVSRVUAVDescriptorHeap();
        CreateDSVDescriptorHeap();
        CreateSamplerDescriptorHeap();
    }

    void D3D12DescriptorContext::CreateRTVDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = Count;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_rtvHeap.Create(m_device, Count, heapDesc);
    }

    void D3D12DescriptorContext::CreateShaderVisibleCBVSRVUAVDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = Count;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        m_shaderVisibleHeap.Create(m_device, Count, desc);
    }

    void D3D12DescriptorContext::CreateCPUOnlyCBVSRVUAVDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = Count;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_cpuOnlyCbvSrvUavHeap.Create(m_device, Count, desc);
    }

    void D3D12DescriptorContext::CreateDSVDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = Count;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_dsvHeap.Create(m_device, Count, desc);
    }

    void D3D12DescriptorContext::CreateSamplerDescriptorHeap(const UINT Count)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = Count;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        m_samplerHeap.Create(m_device, Count, desc);
    }

    GpuDescriptorArena D3D12DescriptorContext::CreateFrameDescriptorArena(const uint32_t frame)
    {

        GpuDescriptorArena arena = {};
        const uint32_t frameArenaBase = PersistentShaderVisibleDescriptorCapacity;
        const uint32_t startIndex = frameArenaBase + frame * ShaderVisibleDescriptorsPerFrame;
        arena.startIndex = startIndex;
        arena.capacity = ShaderVisibleDescriptorsPerFrame;
        arena.used = 0;
        arena.cpuStart = m_shaderVisibleHeap.GetCpuHandleAtIndex(startIndex);
        arena.gpuStart = m_shaderVisibleHeap.GetGpuHandleAtIndex(startIndex);

        return arena;
    }

    CpuDescriptorAllocation D3D12DescriptorContext::AllocateRTV(const uint32_t count)
    {
        if (!m_rtvHeap.HasSpaceForCount(count))
        {
            throw std::runtime_error("RTV descriptor heap is full.");
        }

        CpuDescriptorAllocation allocation = {};
        allocation.count = count;
        m_rtvHeap.Allocate(allocation);
        return allocation;
    }

    CpuDescriptorAllocation D3D12DescriptorContext::AllocateDSV(const uint32_t count)
    {
        if (!m_dsvHeap.HasSpaceForCount(count))
        {
            throw std::runtime_error("DSV descriptor heap is full.");
        }

        CpuDescriptorAllocation allocation = {};
        allocation.count = count;
        m_dsvHeap.Allocate(allocation);
        return allocation;
    }

    GpuDescriptorAllocation D3D12DescriptorContext::AllocateShaderVisibleCBVSRVUAV(const uint32_t count)
    {
        if (m_shaderVisibleHeap.GetUsed() + count > PersistentShaderVisibleDescriptorCapacity)
        {
            throw std::runtime_error("CBV/SRV/UAV heap is full.");
        }

        GpuDescriptorAllocation allocation = {};
        allocation.count = count;
        m_shaderVisibleHeap.Allocate(allocation);
        return allocation;
    }

    CpuDescriptorAllocation D3D12DescriptorContext::AllocateCpuOnlyCBVSRVUAV(const uint32_t count)
    {
        if (!m_cpuOnlyCbvSrvUavHeap.HasSpaceForCount(count))
        {
            throw std::runtime_error("CBV/SRV/UAV heap is full.");
        }

        CpuDescriptorAllocation allocation = {};
        allocation.count = count;
        m_cpuOnlyCbvSrvUavHeap.Allocate(allocation);
        return allocation;
    }

    GpuDescriptorAllocation D3D12DescriptorContext::AllocateSampler(const uint32_t count)
    {
        if (!m_samplerHeap.HasSpaceForCount(count))
        {
            throw std::runtime_error("Sampler descriptor heap is full.");
        }

        GpuDescriptorAllocation allocation = {};
        allocation.count = count;
        m_samplerHeap.Allocate(allocation);
        return allocation;
    }

    GpuDescriptorTable D3D12DescriptorContext::AllocateShaderVisibleTableFromFrameArena(GpuDescriptorArena& arena, uint32_t count)
    {
        if (arena.used + count > arena.capacity)
        {
            throw std::runtime_error("Frame shader-visible descriptor arena is full.");
        }

        const uint32_t localOffset = arena.used;
        arena.used += count;

        GpuDescriptorTable table = {};
        table.startIndex = arena.startIndex + localOffset;
        table.count = count;
        table.offset = localOffset;
        table.cpuStart = m_shaderVisibleHeap.GetCpuHandleAtIndex(table.startIndex);
        table.gpuStart = m_shaderVisibleHeap.GetGpuHandleAtIndex(table.startIndex);

        return table;
    }

    ID3D12DescriptorHeap* D3D12DescriptorContext::GetCBVSRVUAVHeap()const
    {
        return m_shaderVisibleHeap.Get();
    }

    ID3D12DescriptorHeap* D3D12DescriptorContext::GetSamplerHeap() const
    {
        return m_samplerHeap.Get();
    }

    UINT D3D12DescriptorContext::GetCBVSRVUAVDescriptorSize()
    {
        return m_shaderVisibleHeap.GetDescriptorSize();
    }

    void D3D12DescriptorContext::Heap::Create(ID3D12Device* device, uint32_t count, D3D12_DESCRIPTOR_HEAP_DESC desc)
    {
        D3D12_THROW_IF_FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));
        m_descriptorSize = device->GetDescriptorHandleIncrementSize(desc.Type);
        m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
        if(IsShaderVisible())
            m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
        m_capacity = count;
        m_used = 0;
    }

    ID3D12DescriptorHeap* D3D12DescriptorContext::Heap::Get() const
    {
        return m_heap.Get();
    }

    uint32_t D3D12DescriptorContext::Heap::GetUsed() const
    {
        return m_used;
    }

    uint32_t D3D12DescriptorContext::Heap::GetDescriptorSize() const
    {
        return m_descriptorSize;
    }

    bool D3D12DescriptorContext::Heap::HasSpaceForCount(uint32_t count) const
    {
        return count > 0 && (m_used + count <= m_capacity);
    }

    bool D3D12DescriptorContext::Heap::IsShaderVisible() const
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = m_heap->GetDesc();
        return (desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) != 0;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorContext::Heap::GetCpuHandleAtIndex(uint32_t index) const
    {
        if (index >= m_capacity)
        {
            throw std::runtime_error("Descriptor heap CPU handle index out of range.");
        }

        D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStart;
        handle.ptr += static_cast<SIZE_T>(index) * m_descriptorSize;
        return handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorContext::Heap::GetGpuHandleAtIndex(uint32_t index) const
    {
        if (!IsShaderVisible())
        {
            throw std::runtime_error("Tried to get GPU handle from non-shader-visible heap.");
        }

        if (index >= m_capacity)
        {
            throw std::runtime_error("Descriptor heap GPU handle index out of range.");
        }

        D3D12_GPU_DESCRIPTOR_HANDLE handle = m_gpuStart;
        handle.ptr += static_cast<UINT64>(index) * m_descriptorSize;
        return handle;
    }

    void D3D12DescriptorContext::Heap::Allocate(CpuDescriptorAllocation& allocation)
    {
        allocation.index = m_used;
        allocation.cpu = GetCpuHandleAtIndex(allocation.index);
        m_used += allocation.count;
    }

    void D3D12DescriptorContext::Heap::Allocate(GpuDescriptorAllocation& allocation)
    {
        if (!IsShaderVisible())
        {
            throw std::runtime_error("Tried to allocate GPU descriptor from a non-shader-visible heap.");
        }

        allocation.index = m_used;
        allocation.cpu = GetCpuHandleAtIndex(allocation.index);
        allocation.gpu = GetGpuHandleAtIndex(allocation.index);
        m_used += allocation.count;
    }
}