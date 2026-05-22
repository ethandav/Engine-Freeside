#pragma once

#include <d3d12.h>
#include <stdint.h>

namespace efg::d3d12
{
    enum class DescriptorVisibility
    {
        CpuOnly,
        ShaderVisible,
        CpuOnlyAndShaderVisible
    };

    struct CpuDescriptorAllocation
    {
        uint32_t index = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
    };

    struct GpuDescriptorAllocation
    {
        uint32_t index = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};
    };
}