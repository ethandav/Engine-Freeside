#include "..\..\..\include\d3d12\systems\D3D12ShadowSystem.h"
#include "..\..\..\include\d3d12\factories\D3D12TextrureFactory.h"
#include "..\..\..\include\render\Camera.h"
#include "..\..\..\include\render\Lights.h"

namespace efg::d3d12
{
	void D3D12ShadowSystem::Initialize(D3D12TextureFactory* textureFactory)
	{
		m_textureFactory = textureFactory;
	}

	ShadowMapFrameData D3D12ShadowSystem::Update(const FramePacket& scene)
	{
		ShadowMapFrameData output = {};
		EnsureShadowMapResources(scene);
		for (int i = 0; i < scene.directionalLights.size(); i++)
		{
			DirectionalShadowEntry entry = {};
			entry.lightIndex = i;
			entry.lightViewProjection = CalculateDirectionalLightViewProjectionMatrix(scene.directionalLights[i]);
			entry.shadowMap = &m_directionalShadowMaps[i];
			output.directionalShadows.push_back(std::move(entry));
		}

		return output;
	}

	void D3D12ShadowSystem::EnsureShadowMapResources(const FramePacket& scene)
	{
		if (scene.directionalLights.size() > m_directionalShadowMaps.size())
		{
			for (int i = m_directionalShadowMaps.size(); i < scene.directionalLights.size(); i++)
			{
				GpuTexture2D texture = m_textureFactory->CreateDepthBuffer(2048, 2048, true);
				m_directionalShadowMaps.push_back(std::move(texture));
			}
		}

		if (scene.pointLights.size() > m_pointShadowCubes.size())
		{
			for (int i = m_pointShadowCubes.size(); i < scene.pointLights.size(); i++)
			{
				GpuTextureCube texture = m_textureFactory->CreateDepthTextureCube(2048, 2048);
				m_pointShadowCubes.push_back(std::move(texture));
			}
		}
	}
	Freeside::Math::Mat4 D3D12ShadowSystem::CalculateDirectionalLightViewProjectionMatrix(Freeside::Lights::Directional light)
	{
		Freeside::Camera lightCamera;
		Freeside::Math::Vec3 sceneCenter = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
		Freeside::Math::Vec3 lightDir = Freeside::Math::Normalize(light.direction);
		float lightDistance = 50.0f;
		Freeside::Math::Vec3 lightPosition = sceneCenter - lightDir * lightDistance;

		float orthoWidth = 40.0f;
		float orthoHeight = 40.0f;
		float nearZ = 0.1f;
		float farZ = 100.0f;
		lightCamera.LookAt(lightPosition, sceneCenter, Freeside::Math::Vec3(0.0f, 1.0f, 0.0f));

		lightCamera.SetOrthographic(orthoWidth, orthoHeight, nearZ, farZ);
		return Freeside::Math::Transpose(lightCamera.GetViewProjectionMatrix());
	}
}