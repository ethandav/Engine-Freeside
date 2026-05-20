#include "..\..\..\include\d3d12\factories\D3D12ResourceFactory.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"

#include <d3dx12.h>

namespace efg::d3d12
{
    void D3D12ResourceFactory::Initialize(ID3D12Device* device)
    {
        m_device = device;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceFactory::CreateCommittedBufferResource(UINT64 sizeInBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_FLAGS flags)
    {
        auto heapProps = CD3DX12_HEAP_PROPERTIES(heapType);
        auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes, flags);

        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        D3D12_THROW_IF_FAILED(m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initialState, nullptr, IID_PPV_ARGS(resource.GetAddressOf())));

        return resource;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceFactory::CreateCommittedUploadBufferResource(UINT64 sizeInBytes)
    {
        return CreateCommittedBufferResource(sizeInBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceFactory::CreateCommittedDefaultBufferResource(UINT64 sizeInBytes, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_FLAGS flags)
    {
        return CreateCommittedBufferResource(sizeInBytes, D3D12_HEAP_TYPE_DEFAULT, initialState, flags);
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceFactory::CreateCommittedTexture2DResource(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue)
    {
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = width;
        desc.Height = height;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = flags;

        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        D3D12_THROW_IF_FAILED(m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initialState, clearValue, IID_PPV_ARGS(resource.GetAddressOf())));

        return resource;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceFactory::CreateCommittedTextureCubeResource(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue)
    {
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = width;
        desc.Height = height;
        desc.DepthOrArraySize = 6;
        desc.MipLevels = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = flags;

        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        D3D12_THROW_IF_FAILED(m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initialState, clearValue, IID_PPV_ARGS(resource.GetAddressOf())));

        return resource;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceFactory::CreateCommittedDepthTexture2DResource(uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = format;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        return CreateCommittedTexture2DResource(width, height, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue);
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> D3D12ResourceFactory::CreateCommittedDepthTextureCubeResource(uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = format;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        return CreateCommittedTextureCubeResource(width, height, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue);
    }
}