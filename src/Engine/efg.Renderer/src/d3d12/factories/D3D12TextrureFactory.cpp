#include "..\..\..\include\d3d12\factories\D3D12TextrureFactory.h"
#include "..\..\..\include\d3d12\factories\D3D12ResourceFactory.h"
#include "..\..\..\include\d3d12\factories\D3D12DescriptorFactory.h"

namespace efg::d3d12
{
    void D3D12TextureFactory::Initialize(ID3D12Device* device, D3D12ResourceFactory* resourceFactory, D3D12DescriptorFactory* descriptorFactory)
    {
        m_resourceFactory = resourceFactory;
        m_descriptorFactory = descriptorFactory;
        m_device = device;
    }

    GpuTexture2D D3D12TextureFactory::CreateTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        GpuTexture2D texture = {};
        texture.width = width;
        texture.height = height;
        texture.mipLevels = 1;
        texture.format = format;
        m_resourceFactory->CreateCommittedTexture2DResource(&texture, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, nullptr);
        m_descriptorFactory->CreateTexture2DSRV(&texture, texture.format, texture.mipLevels);

        return texture;
    }

    GpuTextureCube D3D12TextureFactory::CreateDepthTextureCube(uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        GpuTextureCube texture = {};
        texture.width = width;
        texture.height = height;
        texture.format = format;

        m_resourceFactory->CreateCommittedDepthTextureCubeResource(&texture);
        m_descriptorFactory->CreateTextureCubeSRV(&texture, DXGI_FORMAT_R32_FLOAT, 1);

        for (uint32_t face = 0; face < 6; ++face)
        {
            m_descriptorFactory->CreateTextureCubeFaceDSV(&texture, DXGI_FORMAT_D32_FLOAT, face);
        }

        return texture;
    }

    GpuTexture2D D3D12TextureFactory::CreateDepthBuffer(uint32_t width, uint32_t height, bool shaderVisible)
    {
        GpuTexture2D texture = {};
        texture.width = width;
        texture.height = height;
        texture.mipLevels = 1;
        texture.format = DXGI_FORMAT_D32_FLOAT;
         
        m_resourceFactory->CreateCommittedDepthTexture2DResource(&texture);

        texture.dsv = m_descriptorFactory->CreateDSV(texture.resource.Get(), nullptr).cpu;

        if (shaderVisible)
        {
            m_descriptorFactory->CreateTexture2DSRV(&texture, DXGI_FORMAT_R32_FLOAT, 1);
        }

        return texture;
    }

    void D3D12TextureFactory::DestroyTexture2D(GpuTexture2D& buffer)
    {
        if (buffer.resource)
        {
            buffer.resource->Unmap(0, nullptr);
        }

        buffer.resource.Reset();
        buffer.cpuSrv.ptr = 0;
        buffer.gpuSrv.ptr = 0; 
    }
}
