#include "..\include\Scene.h"
#include "..\..\efg.Renderer\include\render\Renderer.h"
#include "..\..\efg.Renderer\include\render\types\FramePacket.h"

#include <stdexcept>

namespace Freeside
{
	namespace Scene
	{
		Scene::Scene(std::wstring name) : name(name)
		{
		}

		void Scene::CreateScenefromDefault(float aspectRatio)
		{
			m_camera.LookAt(Freeside::Math::Vec3(0.0f, 1.0f, -5.0f), Freeside::Math::Vec3(0.0f, 0.0f, 0.0f));
			m_camera.SetPerspective(0.78539816339f, aspectRatio, 0.1f, 1000.0f);

			m_dirLight.color = Math::Vec3(1.0f, 1.0f, 1.0f);
			m_dirLight.direction = Math::Vec3(-0.2f, -1.0f, -0.3f);
		}

		SceneRenderObjectHandle Scene::AddRenderObjectToRenderQueue(RenderObject object)
		{
			m_renderObjectQueue.push_back(std::move(object));
			objectCount++;
			return SceneRenderObjectHandle
			{
				static_cast<uint32_t>(m_renderObjectQueue.size() - 1)
			};
		}

		PointLightHandle Scene::AddPointLightToScene(Lights::Point light)
		{
			m_pointLights.push_back(std::move(light));
			pointLightCount++;
			return PointLightHandle
			{
				static_cast<uint32_t>(m_pointLights.size() - 1)
			};
		}

		RenderObject* Scene::GetRenderObjectByHandle(SceneRenderObjectHandle handle)
		{
			if (!handle.IsValid() || handle.index >= m_renderObjectQueue.size())
			{
				throw std::runtime_error("Invalid Object handle.");
			}

			return &m_renderObjectQueue[handle.index];
		}

		Lights::Point* Scene::GetPointLightByHandle(PointLightHandle handle)
		{
			if (!handle.IsValid() || handle.index >= m_pointLights.size())
			{
				throw std::runtime_error("Invalid Point Light handle.");
			}

			return &m_pointLights[handle.index];
		}

		void Scene::AddCamera(Camera camera)
		{
			m_camera = camera;
		}

		void Scene::Render(Renderer* renderer)
		{
			efg::FramePacket renderData = {};
			renderData.renderObjects.reserve(objectCount);
			renderData.pointLights.reserve(pointLightCount);

			renderData.camera = m_camera;
			renderData.directionalLight = m_dirLight;
			renderData.renderObjects = m_renderObjectQueue;
			renderData.pointLights = m_pointLights;

			renderer->SubmitFrame(std::move(renderData));
		}
	}
}
