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

    void D3D12TextureFactory::CreateTexture2D(GpuTexture2D& texture, DescriptorVisibility visibility)
    {
        m_resourceFactory->CreateCommittedTexture2DResource(&texture, texture.flags, texture.initialState, nullptr);

        if (visibility == DescriptorVisibility::ShaderVisible || visibility == DescriptorVisibility::CpuOnlyAndShaderVisible)
        {
            m_descriptorFactory->CreateTexture2DSRV(&texture, visibility);
        }

        if ((texture.flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0)
        {
            m_descriptorFactory->CreateTexture2DRTV(&texture, visibility);
        }
    }

    void D3D12TextureFactory::CreateTextureCube(GpuTextureCube& texture, DescriptorVisibility visibility)
    {
        m_resourceFactory->CreateCommittedTextureCubeResource(&texture, texture.flags, texture.initialState, nullptr);

        if (visibility == DescriptorVisibility::ShaderVisible || visibility == DescriptorVisibility::CpuOnlyAndShaderVisible)
        {
            m_descriptorFactory->CreateTextureCubeSRV(&texture, visibility);
        }

        if ((texture.flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0)
        {
            for (uint32_t face = 0; face < 6; ++face)
            {
                m_descriptorFactory->CreateTextureCubeFaceRTV(&texture, face);
            }
        }
    }

    GpuTextureCube D3D12TextureFactory::CreateDepthTextureCube(uint32_t width, uint32_t height, DescriptorVisibility visibility, DXGI_FORMAT format)
    {
        GpuTextureCube texture = {};
        texture.width = width;
        texture.height = height;
        texture.resourceFormat = format;
        texture.srvFormat = DXGI_FORMAT_R32_FLOAT;
        texture.dsvFormat = DXGI_FORMAT_D32_FLOAT;
        m_resourceFactory->CreateCommittedDepthTextureCubeResource(&texture);
        m_descriptorFactory->CreateTextureCubeSRV(&texture, visibility);

        for (uint32_t face = 0; face < 6; ++face)
        {
            m_descriptorFactory->CreateTextureCubeFaceDSV(&texture, face);
        }

        return texture;
    }

    GpuTexture2D D3D12TextureFactory::CreateDepthBuffer(uint32_t width, uint32_t height, DescriptorVisibility visibility)
    {
        GpuTexture2D texture = {};
        texture.width = width;
        texture.height = height;
        texture.mipLevels = 1;
        texture.resourceFormat = DXGI_FORMAT_D32_FLOAT;
         
        m_resourceFactory->CreateCommittedDepthTexture2DResource(&texture);

        texture.dsv = m_descriptorFactory->CreateDSV(texture.resource.Get(), nullptr).cpu;

        if (visibility == DescriptorVisibility::ShaderVisible || visibility == DescriptorVisibility::CpuOnlyAndShaderVisible)
        {
            m_descriptorFactory->CreateTexture2DSRV(&texture, visibility);
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
