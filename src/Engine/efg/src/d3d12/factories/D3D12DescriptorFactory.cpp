#include "..\..\..\include\d3d12\factories\D3D12DescriptorFactory.h"
#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"

namespace efg::d3d12
{
    static uint32_t AlignConstantBufferSize(uint32_t size)
    {
        return (size + 255) & ~255;
    }

	void D3D12DescriptorFactory::Initialize(ID3D12Device* device, D3D12DescriptorContext* descriptorContext)
	{
		m_device = device;
		m_descriptorContext = descriptorContext;
	}

    CpuDescriptorAllocation D3D12DescriptorFactory::CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc)
    {
        CpuDescriptorAllocation allocation = m_descriptorContext->AllocateRTV();
        m_device->CreateRenderTargetView(resource, desc, allocation.cpu);

        return allocation;
    }

    CpuDescriptorAllocation D3D12DescriptorFactory::CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
    {
        CpuDescriptorAllocation allocation = m_descriptorContext->AllocateDSV();
        m_device->CreateDepthStencilView(resource, desc, allocation.cpu);

        return allocation;
    }

    GpuDescriptorAllocation D3D12DescriptorFactory::CreateCBV(ID3D12Resource* resource, uint32_t sizeInBytes)
    {
        GpuDescriptorAllocation allocation = m_descriptorContext->AllocateShaderVisibleCBVSRVUAV();

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = AlignConstantBufferSize(sizeInBytes);
        m_device->CreateConstantBufferView(&cbvDesc, allocation.cpu);

        return allocation;
    }

    GpuDescriptorAllocation D3D12DescriptorFactory::CreateUAV(ID3D12Resource* resource, uint32_t elementCount, uint32_t elementStride, ID3D12Resource* counterResource)
    {
        GpuDescriptorAllocation allocation = m_descriptorContext->AllocateShaderVisibleCBVSRVUAV();

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = elementCount;
        uavDesc.Buffer.StructureByteStride = elementStride;
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
        m_device->CreateUnorderedAccessView(resource, counterResource, &uavDesc, allocation.cpu);

        return allocation;
    }

    void D3D12DescriptorFactory::CreateStructuredBufferSRV(GpuStructuredBuffer* buffer, uint32_t elementCount, uint32_t elementStride, DescriptorVisibility visibility)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc = BuildStructuredBufferSRVDesc(elementCount, elementStride);
        CreateSRVWithVisibility(buffer->resource.Get(), desc, visibility, &buffer->cpuSrv, &buffer->gpuSrv);
    }

    void efg::d3d12::D3D12DescriptorFactory::CreateTexture2DSRV(GpuTexture2D* texture, DescriptorVisibility visibility)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc = BuildTexture2DSRVDesc(texture->srvFormat, texture->mipLevels);
        texture->bindlessSrvIndex = CreateSRVWithVisibility(texture->resource.Get(), desc, visibility, &texture->cpuSrv, &texture->gpuSrv);
    }

    void D3D12DescriptorFactory::CreateTexture2DRTV(GpuTexture2D* texture, DescriptorVisibility visibility)
    {
        D3D12_RENDER_TARGET_VIEW_DESC desc = BuildTexture2DRTVDesc(texture->rtvFormat);

        CpuDescriptorAllocation allocation = CreateRTV(texture->resource.Get(), &desc);
        texture->rtv = allocation.cpu;
    }

    void efg::d3d12::D3D12DescriptorFactory::CreateTextureCubeSRV(GpuTextureCube* texture, DescriptorVisibility visibility)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc = BuildTextureCubeSRVDesc(texture->srvFormat, texture->mipLevels);
        texture->bindlessSrvIndex = CreateSRVWithVisibility(texture->resource.Get(), desc, visibility, &texture->cpuSrv, &texture->gpuSrv);
    }

    void D3D12DescriptorFactory::CreateTextureCubeFaceDSV(GpuTextureCube* texture, uint32_t faceIndex)
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
        desc.Format = texture->dsvFormat;
        desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
        desc.Flags = D3D12_DSV_FLAG_NONE;
        desc.Texture2DArray.MipSlice = 0;
        desc.Texture2DArray.FirstArraySlice = faceIndex;
        desc.Texture2DArray.ArraySize = 1;
        CpuDescriptorAllocation allocation = CreateDSV(texture->resource.Get(), &desc);
        texture->dsv[faceIndex] = allocation.cpu;
    }

    void D3D12DescriptorFactory::CreateTextureCubeFaceRTV(GpuTextureCube* texture, uint32_t faceIndex)
    {
        D3D12_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = texture->rtvFormat;
        desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        desc.Texture2DArray.MipSlice = 0;
        desc.Texture2DArray.FirstArraySlice = faceIndex;
        desc.Texture2DArray.ArraySize = 1;
        CpuDescriptorAllocation allocation = CreateRTV(texture->resource.Get(), &desc);
        texture->dsv[faceIndex] = allocation.cpu;
    }

    GpuDescriptorAllocation D3D12DescriptorFactory::CreateSampler(const D3D12_SAMPLER_DESC& samplerDesc)
    {
        GpuDescriptorAllocation allocation = m_descriptorContext->AllocateSampler();
        m_device->CreateSampler(&samplerDesc, allocation.cpu);

        return allocation;
    }

    uint32_t D3D12DescriptorFactory::CreateSRVWithVisibility(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc, DescriptorVisibility visibility, D3D12_CPU_DESCRIPTOR_HANDLE* outCpuOnly, D3D12_GPU_DESCRIPTOR_HANDLE* outShaderVisible)
    {
        if (outCpuOnly)
        {
            *outCpuOnly = {};
        }

        if (outShaderVisible)
        {
            *outShaderVisible = {};
        }

        if (visibility == DescriptorVisibility::CpuOnly || visibility == DescriptorVisibility::CpuOnlyAndShaderVisible)
        {
            auto cpuOnlyAllocation = m_descriptorContext->AllocateCpuOnlyCBVSRVUAV();
            m_device->CreateShaderResourceView(resource, &desc, cpuOnlyAllocation.cpu);

            if (outCpuOnly)
            {
                *outCpuOnly = cpuOnlyAllocation.cpu;
            }
        }

        if (visibility == DescriptorVisibility::ShaderVisible || visibility == DescriptorVisibility::CpuOnlyAndShaderVisible)
        {
            auto shaderVisibleAllocation = m_descriptorContext->AllocateShaderVisibleCBVSRVUAV();
            m_device->CreateShaderResourceView(resource, &desc, shaderVisibleAllocation.cpu);

            if (outShaderVisible)
            {
                *outShaderVisible = shaderVisibleAllocation.gpu;
            }

            return shaderVisibleAllocation.index;
        }

        return -1;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC D3D12DescriptorFactory::BuildStructuredBufferSRVDesc(uint32_t elementCount, uint32_t elementStride)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = elementCount;
        desc.Buffer.StructureByteStride = elementStride;
        desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        return desc;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC D3D12DescriptorFactory::BuildTexture2DSRVDesc(DXGI_FORMAT format, uint32_t mipLevels)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.Format = format;
        desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = mipLevels;
        desc.Texture2D.MostDetailedMip = 0;
        desc.Texture2D.PlaneSlice = 0;
        desc.Texture2D.ResourceMinLODClamp = 0.0f;

        return desc;
    }

    D3D12_RENDER_TARGET_VIEW_DESC D3D12DescriptorFactory::BuildTexture2DRTVDesc(DXGI_FORMAT format)
    {
        D3D12_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = format;
        desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;
        desc.Texture2D.PlaneSlice = 0;

        return desc;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC D3D12DescriptorFactory::BuildTextureCubeSRVDesc(DXGI_FORMAT format, uint32_t mipLevels)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = format;
        desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.TextureCube.MostDetailedMip = 0;
        desc.TextureCube.MipLevels = mipLevels;
        desc.TextureCube.ResourceMinLODClamp = 0.0f;

        return desc;
    }
}