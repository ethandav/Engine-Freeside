#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

namespace efg::d3d12
{
    struct GpuTexture2D
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t mipLevels = 1;
        DXGI_FORMAT resourceFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT dsvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuSrv = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuSrv = {};
        D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
        D3D12_CPU_DESCRIPTOR_HANDLE rtv = {};

        int32_t bindlessSrvIndex = -1;

        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
    };

    struct GpuTextureCube
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        uint32_t width = 0;
        uint32_t height = 0;
        DXGI_FORMAT resourceFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuSrv = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuSrv = {};
        D3D12_CPU_DESCRIPTOR_HANDLE dsv[6] = {};
        D3D12_CPU_DESCRIPTOR_HANDLE rtv[6] = {};

        int32_t bindlessSrvIndex = -1;

        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
    };
}