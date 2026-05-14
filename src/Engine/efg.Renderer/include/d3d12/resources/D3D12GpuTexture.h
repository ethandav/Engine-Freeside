#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

namespace efg::d3d12
{
    struct GpuTexture2D
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuSrv = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuSrv = {};

        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t mipLevels = 1;
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    };

    struct GpuDepthBuffer
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
        DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT;
    };
}