#pragma once
#include <cstdint>

namespace efg
{
    enum class Format
    {
        Unknown = 0,

        // 8-bit normalized
        R8_UNorm,
        R8G8_UNorm,
        R8G8B8A8_UNorm,
        R8G8B8A8_UNorm_SRGB,

        B8G8R8A8_UNorm,
        B8G8R8A8_UNorm_SRGB,

        // 8-bit integer
        R8_UInt,
        R8G8_UInt,
        R8G8B8A8_UInt,

        // 16-bit float/integer
        R16_Float,
        R16G16_Float,
        R16G16B16A16_Float,

        R16_UInt,
        R16G16_UInt,
        R16G16B16A16_UInt,

        // 32-bit float/integer
        R32_Float,
        R32G32_Float,
        R32G32B32_Float,
        R32G32B32A32_Float,

        R32_UInt,
        R32G32_UInt,
        R32G32B32_UInt,
        R32G32B32A32_UInt,

        // Depth/stencil
        D32_Float,
        D24_UNorm_S8_UInt,
        D32_Float_S8X24_UInt
    };
}