#include "..\..\..\include\d3d12\factories\D3D12TextrureFactory.h"
#include "..\..\..\include\d3d12\factories\D3D12ResourceFactory.h"

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

    GpuDepthBuffer D3D12TextureFactory::CreateDepthBuffer(uint32_t width, uint32_t height)
    {
        GpuDepthBuffer buffer = {};
        buffer.resource = m_resourceFactory->CreateCommittedDepthTexture2DResource(width, height);

        return buffer;
    }

    void D3D12TextureFactory::DestroyDepthBuffer(GpuDepthBuffer& buffer)
    {
        if (buffer.resource)
        {
            buffer.resource->Unmap(0, nullptr);
        }

        buffer.resource.Reset();
        buffer.dsv.ptr = 0;
    }
}
