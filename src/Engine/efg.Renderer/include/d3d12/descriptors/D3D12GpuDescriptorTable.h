#pragma once
#include <cstdint>
#include <d3d12.h>

namespace efg::d3d12
{
    struct GpuDescriptorTable
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuStart = {};
        uint32_t count = 0;
        uint32_t startIndex = 0;
    };
}