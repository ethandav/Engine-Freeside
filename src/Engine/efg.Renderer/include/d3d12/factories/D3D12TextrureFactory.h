#pragma once
#include "../resources/D3D12GpuTexture.h"

#include <stdint.h>
#include <dxgi1_6.h>

struct ID3D12Device;

namespace efg::d3d12
{
	class D3D12ResourceFactory;

	class D3D12TextureFactory
	{
	public:
		void Initialize(ID3D12Device* device, D3D12ResourceFactory* resourceFactory);
		GpuTexture2D CreateTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format);
		GpuDepthBuffer CreateDepthBuffer(uint32_t width, uint32_t height);
		void DestroyDepthBuffer(GpuDepthBuffer& buffer);

	private:
		ID3D12Device* m_device = nullptr;
		D3D12ResourceFactory* m_resourceFactory = nullptr;
	};
}