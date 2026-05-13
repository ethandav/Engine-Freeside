#include "..\..\include\d3d12\D3D12Format.h"

namespace efg::d3d12
{
    DXGI_FORMAT ToDxgiFormat(efg::Format format)
    {
        switch (format)
        {
        case efg::Format::R8_UNorm:
            return DXGI_FORMAT_R8_UNORM;

        case efg::Format::R8G8_UNorm:
            return DXGI_FORMAT_R8G8_UNORM;

        case efg::Format::R8G8B8A8_UNorm:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case efg::Format::R8G8B8A8_UNorm_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        case efg::Format::B8G8R8A8_UNorm:
            return DXGI_FORMAT_B8G8R8A8_UNORM;

        case efg::Format::B8G8R8A8_UNorm_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

        case efg::Format::R16_Float:
            return DXGI_FORMAT_R16_FLOAT;

        case efg::Format::R16G16_Float:
            return DXGI_FORMAT_R16G16_FLOAT;

        case efg::Format::R16G16B16A16_Float:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case efg::Format::R32_Float:
            return DXGI_FORMAT_R32_FLOAT;

        case efg::Format::R32G32_Float:
            return DXGI_FORMAT_R32G32_FLOAT;

        case efg::Format::R32G32B32_Float:
            return DXGI_FORMAT_R32G32B32_FLOAT;

        case efg::Format::R32G32B32A32_Float:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case efg::Format::R32_UInt:
            return DXGI_FORMAT_R32_UINT;

        case efg::Format::R32G32_UInt:
            return DXGI_FORMAT_R32G32_UINT;

        case efg::Format::R32G32B32_UInt:
            return DXGI_FORMAT_R32G32B32_UINT;

        case efg::Format::R32G32B32A32_UInt:
            return DXGI_FORMAT_R32G32B32A32_UINT;

        case efg::Format::D32_Float:
            return DXGI_FORMAT_D32_FLOAT;

        case efg::Format::D24_UNorm_S8_UInt:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;

        case efg::Format::D32_Float_S8X24_UInt:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }
}