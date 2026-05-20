#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <stdint.h>

namespace efg::d3d12
{
    class D3D12ResourceFactory
    {
    public:
        void Initialize(ID3D12Device* device);

        Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedBufferResource(UINT64 sizeInBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedUploadBufferResource(UINT64 sizeInBytes);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedDefaultBufferResource(UINT64 sizeInBytes, D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedTexture2DResource(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue = nullptr);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedTextureCubeResource(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue = nullptr);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedDepthTexture2DResource(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedDepthTextureCubeResource(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS);

    private:
        ID3D12Device* m_device = nullptr;
    };
}