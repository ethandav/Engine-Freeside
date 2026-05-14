#include "..\..\..\include\render\types\FormatUtils.h"

namespace efg
{
    uint32_t BytesPerPixelOrBlock(Format format)
    {
        switch (format)
        {
        case Format::R8_UNorm:
        case Format::R8_UInt:
            return 1;

        case Format::R8G8_UNorm:
        case Format::R8G8_UInt:
        case Format::R16_Float:
        case Format::R16_UInt:
            return 2;

        case Format::R8G8B8A8_UNorm:
        case Format::R8G8B8A8_UNorm_SRGB:
        case Format::B8G8R8A8_UNorm:
        case Format::B8G8R8A8_UNorm_SRGB:
        case Format::R16G16_Float:
        case Format::R16G16_UInt:
        case Format::R32_Float:
        case Format::R32_UInt:
        case Format::D32_Float:
        case Format::D24_UNorm_S8_UInt:
            return 4;

        case Format::R16G16B16A16_Float:
        case Format::R16G16B16A16_UInt:
        case Format::R32G32_Float:
        case Format::R32G32_UInt:
        case Format::D32_Float_S8X24_UInt:
            return 8;

        case Format::R32G32B32_Float:
        case Format::R32G32B32_UInt:
            return 12;

        case Format::R32G32B32A32_Float:
        case Format::R32G32B32A32_UInt:
            return 16;

        default:
            return 0;
        }
    }
}