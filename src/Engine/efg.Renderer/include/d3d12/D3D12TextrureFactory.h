#pragma once
#include "D3D12ResourceFactory.h"
#include "D3D12BufferData.h"


namespace efg::d3d12
{
	class D3D12TextureFactory
	{
	public:
		void Initialize(ID3D12Device* device, D3D12ResourceFactory* resourceFactory);
		GpuTexture2D CreateTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format);
	private:
		ID3D12Device* m_device = nullptr;
		D3D12ResourceFactory* m_resourceFactory = nullptr;
	};
}