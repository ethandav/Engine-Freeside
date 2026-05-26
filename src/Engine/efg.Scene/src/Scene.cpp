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

		DirectionalLightHandle Scene::AddDirectionalLightToScene(Lights::Directional light)
		{
			m_directionalLights.push_back(std::move(light));
			directionalLightCount++;
			return DirectionalLightHandle
			{
				static_cast<uint32_t>(m_directionalLights.size() - 1)
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

		Lights::Directional* Scene::GetDirectionalLightByHandle(DirectionalLightHandle handle)
		{
			if (!handle.IsValid() || handle.index >= m_directionalLights.size())
			{
				throw std::runtime_error("Invalid Directional Light handle.");
			}

			return &m_directionalLights[handle.index];
		}

		Lights::Point* Scene::GetPointLightByHandle(PointLightHandle handle)
		{
			if (!handle.IsValid() || handle.index >= m_pointLights.size())
			{
				throw std::runtime_error("Invalid Point Light handle.");
			}

			return &m_pointLights[handle.index];
		}

		CameraHandle Scene::AddCamera(Camera camera)
		{
			m_cameras.push_back(camera);
			return CameraHandle
			{
				static_cast<uint32_t>(m_cameras.size() - 1)
			};
		}

		Camera* Scene::GetCameraByHandle(CameraHandle handle)
		{
			if (!handle.IsValid() || handle.index >= m_cameras.size())
			{
				throw std::runtime_error("Invalid Camera handle.");
			}

			return &m_cameras[handle.index];
		}

		void Scene::SetActiveCamera(CameraHandle handle)
		{
			if (!handle.IsValid() || handle.index >= m_cameras.size())
			{
				throw std::runtime_error("Invalid Camera handle.");
			}
			m_ActiveCamera = &m_cameras[handle.index];
		}

		void Scene::Render(Renderer* renderer)
		{
			efg::FramePacket renderData = {};
			renderData.renderObjects.reserve(objectCount);
			renderData.pointLights.reserve(pointLightCount);
			renderData.directionalLights.reserve(directionalLightCount);

			renderData.camera = *m_ActiveCamera;
			renderData.directionalLights = m_directionalLights;
			renderData.renderObjects = m_renderObjectQueue;
			renderData.pointLights = m_pointLights;

			renderer->SubmitFrame(std::move(renderData));
		}
	}
}
