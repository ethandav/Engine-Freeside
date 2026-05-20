#pragma once
#include "..\..\d3d12\resources\D3D12GpuTexture.h"
#include "..\..\..\include\render\types\FramePacket.h"

#include <vector>

namespace efg::d3d12
{
	class D3D12TextureFactory;

	class D3D12ShadowSystem
	{
	public:
		void Initialize(D3D12TextureFactory* textureFactory);
		void Update(const FramePacket& scene);
	private:
		D3D12TextureFactory* m_textureFactory = nullptr;
		std::vector<GpuTexture2D> m_directionalShadowMaps;
		std::vector<GpuTextureCube> m_pointShadowCubes;
	};
}