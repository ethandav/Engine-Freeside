#include "..\..\include\d3d12\D3D12TextrureFactory.h"
#include "..\..\include\d3d12\D3D12Error.h"
#include <d3dx12.h>
#include <stdexcept>

namespace efg::d3d12
{
    void D3D12TextureFactory::Initialize(ID3D12Device* device, D3D12ResourceFactory* resourceFactory)
    {
        m_resourceFactory = resourceFactory;
        m_device = device;
    }

    GpuTexture2D D3D12TextureFactory::CreateTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        GpuTexture2D texture = {};
        texture.width = width;
        texture.height = height;
        texture.mipLevels = 1;
        texture.format = format;

        texture.resource = m_resourceFactory->CreateCommittedTexture2DResource(width, height, format, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, nullptr);

        return texture;
    }
}
