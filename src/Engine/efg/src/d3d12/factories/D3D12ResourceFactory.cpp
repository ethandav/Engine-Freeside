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

    void D3D12ResourceFactory::CreateCommittedTexture2DResource(GpuTexture2D* texture, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue)
    {
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = texture->width;
        desc.Height = texture->height;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = texture->resourceFormat;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = flags;
        D3D12_THROW_IF_FAILED(m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initialState, clearValue, IID_PPV_ARGS(texture->resource.GetAddressOf())));
    }

    void D3D12ResourceFactory::CreateCommittedTextureCubeResource(GpuTextureCube* texture, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue)
    {
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = texture->width;
        desc.Height = texture->height;
        desc.DepthOrArraySize = 6;
        desc.MipLevels = 1;
        desc.Format = texture->resourceFormat;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = flags;
        D3D12_THROW_IF_FAILED(m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initialState, clearValue, IID_PPV_ARGS(texture->resource.GetAddressOf())));
    }

    void D3D12ResourceFactory::CreateCommittedDepthTexture2DResource(GpuTexture2D* texture)
    {
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        CreateCommittedTexture2DResource(texture, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue);
    }

    void D3D12ResourceFactory::CreateCommittedDepthTextureCubeResource(GpuTextureCube* texture)
    {
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        CreateCommittedTextureCubeResource(texture, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue);
    }
}