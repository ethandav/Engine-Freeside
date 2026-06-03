#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

namespace efg::d3d12
{
    struct GpuBuffer
    {
        uint64_t sizeInBytes = 0;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    };

    struct GpuConstantBuffer
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        void* mappedData = nullptr;

        uint64_t sizeInBytes = 0;
        uint64_t alignedSizeInBytes = 0;
    };

    struct GpuStructuredBuffer
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        uint8_t* mappedData = nullptr;

        uint32_t elementCount = 0;
        uint32_t elementStride = 0;
        uint64_t sizeInBytes = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuSrv = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuSrv = {};
    };
}