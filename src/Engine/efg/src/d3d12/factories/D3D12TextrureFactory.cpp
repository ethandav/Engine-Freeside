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
            m_descriptorFactory->CreateTexture2DSRV(&texture, texture.resourceFormat, texture.mipLevels, visibility);
        }

        if ((texture.flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0)
        {
            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
            rtvDesc.Format = texture.rtvFormat;
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2D.MipSlice = 0;
            rtvDesc.Texture2D.PlaneSlice = 0;
            m_descriptorFactory->CreateRTV(texture.resource.Get(), &rtvDesc);
        }
    }

    GpuTextureCube D3D12TextureFactory::CreateTextureCube(uint32_t width, uint32_t height, DescriptorVisibility visibility, DXGI_FORMAT format)
    {
        GpuTextureCube texture = {};
        texture.width = width;
        texture.height = height;
        texture.resourceFormat = format;
        m_resourceFactory->CreateCommittedTextureCubeResource(&texture, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, nullptr);
        m_descriptorFactory->CreateTextureCubeSRV(&texture, format, 1, visibility);

        return texture;
    }

    GpuTextureCube D3D12TextureFactory::CreateDepthTextureCube(uint32_t width, uint32_t height, DescriptorVisibility visibility, DXGI_FORMAT format)
    {
        GpuTextureCube texture = {};
        texture.width = width;
        texture.height = height;
        texture.resourceFormat = format;
        m_resourceFactory->CreateCommittedDepthTextureCubeResource(&texture);
        m_descriptorFactory->CreateTextureCubeSRV(&texture, DXGI_FORMAT_R32_FLOAT, 1, visibility);

        for (uint32_t face = 0; face < 6; ++face)
        {
            m_descriptorFactory->CreateTextureCubeFaceDSV(&texture, DXGI_FORMAT_D32_FLOAT, face);
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
            m_descriptorFactory->CreateTexture2DSRV(&texture, DXGI_FORMAT_R32_FLOAT, 1, visibility);
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
