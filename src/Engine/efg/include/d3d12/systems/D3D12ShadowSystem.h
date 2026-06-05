#pragma once
#include "..\resources\D3D12GpuTexture.h"
#include "..\types\ShadowFrameData.h"
#include "..\..\render\types\FramePacket.h"

#include <vector>

namespace efg::d3d12
{
	class D3D12TextureFactory;

	class D3D12ShadowSystem
	{
	public:
		void Initialize(D3D12TextureFactory* textureFactory);
		ShadowMapFrameData Update(const FramePacket& scene);
	private:
		void EnsureShadowMapResources(const FramePacket& scene);
		Freeside::Math::Mat4 CalculateDirectionalLightViewProjectionMatrix(Freeside::Lights::Directional light);
		void CalculatePointLightViewProjectionMatrices(const Freeside::Lights::Point& light, Freeside::Math::Mat4 outFaceViewProjection[6], float farPlane);

		D3D12TextureFactory* m_textureFactory = nullptr;
		std::vector<GpuTexture2D> m_directionalShadowMaps;
		std::vector<GpuTextureCube> m_pointShadowCubes;
	};
}