#pragma once

#include <wrl.h>
#include <cstdint>
#include <d3d12.h>

namespace efg::d3d12
{
    struct GpuConstantBufferArena
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        uint8_t* mappedData = nullptr;

        uint64_t capacityInBytes = 0;
        uint64_t currentOffset = 0;

        void Reset()
        {
            currentOffset = 0;
        }

        D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress(uint64_t offset) const
        {
            return resource->GetGPUVirtualAddress() + offset;
        }
    };

    struct GpuUploadBufferArena
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        uint8_t* mappedData = nullptr;

        uint64_t capacityInBytes = 0;
        uint64_t currentOffset = 0;

        D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress(uint64_t offset) const
        {
            return resource->GetGPUVirtualAddress() + offset;
        }

        void Reset()
        {
            currentOffset = 0;
        }
    };

    struct GpuUploadBufferAllocation
    {
        uint8_t* cpu = nullptr;
        D3D12_GPU_VIRTUAL_ADDRESS gpu = 0;
        uint64_t sizeInBytes = 0;
        uint64_t offset = 0;
    };
}