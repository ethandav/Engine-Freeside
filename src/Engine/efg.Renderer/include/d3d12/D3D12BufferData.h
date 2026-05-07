#pragma once

using Microsoft::WRL::ComPtr;

static constexpr UINT64 ConstantArenaSize = 1024 * 256;

struct GpuBuffer
{
    UINT64 sizeInBytes = 0;
    ComPtr<ID3D12Resource> resource;
    ComPtr<ID3D12Resource> uploadResource;
};

struct GpuConstantBuffer
{
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    void* mappedData = nullptr;

    UINT64 sizeInBytes = 0;
    UINT64 alignedSizeInBytes = 0;
};

inline UINT64 AlignUp(UINT64 value, UINT64 alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

inline UINT64 AlignConstantBufferSize(UINT64 size)
{
    return AlignUp(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
}

struct GpuConstantBufferArena
{
    ComPtr<ID3D12Resource> resource;

    uint8_t* mappedData = nullptr;

    UINT64 capacityInBytes = 0;
    UINT64 currentOffset = 0;

    void Reset()
    {
        currentOffset = 0;
    }

    D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress(UINT64 offset) const
    {
        return resource->GetGPUVirtualAddress() + offset;
    }
};