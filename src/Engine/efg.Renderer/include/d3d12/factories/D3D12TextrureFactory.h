#pragma once
#include "../resources/D3D12GpuTexture.h"

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
		GpuTexture2D CreateTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format);
		GpuTextureCube CreateTextureCube(uint32_t width, uint32_t height, DXGI_FORMAT format);
		GpuTexture2D CreateDepthBuffer(uint32_t width, uint32_t height, bool shaderVisible = false);
		void DestroyTexture2D(GpuTexture2D& buffer);

	private:
		ID3D12Device* m_device = nullptr;
		D3D12ResourceFactory* m_resourceFactory = nullptr;
		D3D12DescriptorFactory* m_descriptorFactory = nullptr;
	};
}