#pragma once

#include <cstdint>
#include <d3d12.h>

namespace efg::d3d12
{

    static constexpr uint64_t InstanceDataAlignment = 16;
    static constexpr uint64_t StructuredBufferAlignment = 16;

    inline uint64_t AlignUp(uint64_t value, uint64_t alignment)
    {
        return (value + alignment - 1) & ~(alignment - 1);
    }

    inline uint64_t AlignConstantBufferSize(uint64_t size)
    {
        return AlignUp(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
    }
}