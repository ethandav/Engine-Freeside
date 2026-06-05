#pragma once
#include "../resources/D3D12GpuTexture.h"
#include "..\descriptors\D3D12DescriptorAllocation.h"

#include <stdint.h>
#include <dxgi1_6.h>

struct ID3D12Device;

namespace efg::d3d12
{
	class D3D12ResourceFactory;
	class D3D12DescriptorFactory;

	class D3D12TextureFactory
	{
	public:
		void Initialize(ID3D12Device* device, D3D12ResourceFactory* resourceFactory, D3D12DescriptorFactory* descriptorFactory);
		void CreateTexture2D(GpuTexture2D& texture, DescriptorVisibility visibility);
		void CreateTextureCube(GpuTextureCube& textureCube, DescriptorVisibility visibility);
		GpuTextureCube CreateDepthTextureCube(uint32_t width, uint32_t height, DescriptorVisibility visibility, DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS);
		GpuTexture2D CreateDepthBuffer(uint32_t width, uint32_t height, DescriptorVisibility visibility);
		void DestroyTexture2D(GpuTexture2D& buffer);

	private:
		ID3D12Device* m_device = nullptr;
		D3D12ResourceFactory* m_resourceFactory = nullptr;
		D3D12DescriptorFactory* m_descriptorFactory = nullptr;
	};
}