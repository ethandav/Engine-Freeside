#pragma once

#include "..\..\..\efg.Core\include\math\Mat4.h"

using Microsoft::WRL::ComPtr;

namespace efg::d3d12
{
    static constexpr UINT64 ConstantArenaSize = 1024 * 256;

    static constexpr UINT64 InstanceDataAlignment = 16;

    inline UINT64 AlignUp(UINT64 value, UINT64 alignment)
    {
        return (value + alignment - 1) & ~(alignment - 1);
    }

    inline UINT64 AlignConstantBufferSize(UINT64 size)
    {
        return AlignUp(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
    }

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

    struct GpuDepthBuffer
    {
        ComPtr<ID3D12Resource> resource;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuSrv = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuSrv = {};
        DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT;
    };

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

    struct InstanceData
    {
        efg::Math::Mat4 world;
    };

    struct GpuUploadBufferArena
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        uint8_t* mappedData = nullptr;

        UINT64 capacityInBytes = 0;
        UINT64 currentOffset = 0;

        D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress(UINT64 offset) const
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
        UINT64 sizeInBytes = 0;
        UINT64 offset = 0;
    };
}