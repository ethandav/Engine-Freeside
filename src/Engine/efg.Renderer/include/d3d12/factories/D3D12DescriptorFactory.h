#pragma once

#include "..\descriptors\D3D12DescriptorAllocation.h"
#include "..\resources\D3D12GpuTexture.h"
#include "..\resources\D3D12GpuBuffer.h"

#include <d3d12.h>

namespace efg::d3d12
{
	class D3D12DescriptorContext;

	enum class DescriptorVisibility
	{
		CpuOnly,
		ShaderVisible,
		CpuOnlyAndShaderVisible
	};

	class D3D12DescriptorFactory
	{
	public:
		void Initialize(ID3D12Device* device, D3D12DescriptorContext* descriptorContext);
		CpuDescriptorAllocation CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);
		CpuDescriptorAllocation CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc);
		GpuDescriptorAllocation CreateCBV(ID3D12Resource* resource, uint32_t sizeInBytes);
		GpuDescriptorAllocation CreateUAV(ID3D12Resource* resource, uint32_t elementCount, uint32_t elementStride, ID3D12Resource* counterResource);
		void CreateStructuredBufferSRV(GpuStructuredBuffer* resource, uint32_t elementCount, uint32_t elementStride);
		void CreateTexture2DSRV(GpuTexture2D* texture, DXGI_FORMAT format, uint32_t mipLevels, DescriptorVisibility visibility = DescriptorVisibility::CpuOnlyAndShaderVisible);
		void CreateTextureCubeSRV(GpuTextureCube* texture, DXGI_FORMAT format, uint32_t mipLevels, DescriptorVisibility visibility = DescriptorVisibility::CpuOnlyAndShaderVisible);
		void CreateTextureCubeFaceDSV(GpuTextureCube* texture, DXGI_FORMAT format, uint32_t faceIndex);
		GpuDescriptorAllocation CreateSampler(const D3D12_SAMPLER_DESC& samplerDesc);
	private:
		D3D12_SHADER_RESOURCE_VIEW_DESC BuildTexture2DSRVDesc(DXGI_FORMAT format, uint32_t mipLevels);
		D3D12_SHADER_RESOURCE_VIEW_DESC BuildTextureCubeSRVDesc(DXGI_FORMAT format, uint32_t mipLevels);

		ID3D12Device* m_device = nullptr;
		D3D12DescriptorContext* m_descriptorContext = nullptr;
	};
}