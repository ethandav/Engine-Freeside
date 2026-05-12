#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

namespace efg::d3d12
{
    class D3D12ResourceFactory
    {
    public:
        void Initialize(ID3D12Device* device);

        Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT64 sizeInBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadBuffer(UINT64 sizeInBytes);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(UINT64 sizeInBytes, D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue = nullptr);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);

    private:
        ID3D12Device* m_device = nullptr;
    };
}