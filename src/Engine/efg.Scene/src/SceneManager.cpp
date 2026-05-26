#include "..\include\SceneManager.h"
#include "..\..\efg.Renderer\include\render\Renderer.h"

#include <stdexcept>

namespace Freeside
{
	namespace Scene
	{
		SceneManager::SceneManager()
		{
		}

		SceneManager::~SceneManager()
		{
		}

		void SceneManager::Initialize(Renderer* renderer)
		{
			m_renderer = renderer;
		}

		SceneHandle SceneManager::CreateScene(std::wstring sceneName)
		{
			Scene newScene(sceneName);
			newScene.CreateScenefromDefault(m_renderer->GetRendererAspectRatio());
			SceneHandle handle = AddSceneToQueue(newScene);
			return handle;
		}

		SceneHandle SceneManager::AddSceneToQueue(const Scene& scene)
		{
			m_sceneQueue.push_back(scene);
			return SceneHandle
			{
				static_cast<uint32_t>(m_sceneQueue.size() - 1)
			};
		}

		SceneRenderObjectHandle SceneManager::AddRenderObjectToRenderQueue(SceneHandle handle, RenderObject& object)
		{
			Scene* scene = GetSceneByHandle(handle);
			return scene->AddRenderObjectToRenderQueue(object);
		}

		PointLightHandle SceneManager::AddPointLightToScene(SceneHandle handle, Lights::Point& light)
		{
			Scene* scene = GetSceneByHandle(handle);
			return scene->AddPointLightToScene(light);
		}

		RenderObject* SceneManager::GetRenderObjectByHandle(SceneHandle sceneHandle, SceneRenderObjectHandle objectHandle)
		{
			Scene* scene = GetSceneByHandle(sceneHandle);
			return scene->GetRenderObjectByHandle(objectHandle);
		}

		DirectionalLightHandle SceneManager::AddDirectionalLightToScene(SceneHandle handle, Lights::Directional& light)
		{
			Scene* scene = GetSceneByHandle(handle);
			return scene->AddDirectionalLightToScene(light);
		}

		Lights::Directional* Freeside::Scene::SceneManager::GetDirectionalLightByHandle(SceneHandle sceneHandle, DirectionalLightHandle handle)
		{
			Scene* scene = GetSceneByHandle(sceneHandle);
			return scene->GetDirectionalLightByHandle(handle);
		}

		Lights::Point* SceneManager::GetPointLightByHandle(SceneHandle sceneHandle, PointLightHandle pointLightHandle)
		{
			Scene* scene = GetSceneByHandle(sceneHandle);
			return scene->GetPointLightByHandle(pointLightHandle);
		}

		CameraHandle SceneManager::AddCamera(SceneHandle handle, Camera camera)
		{
			Scene* scene = GetSceneByHandle(handle);
			return scene->AddCamera(camera);
		}

		Camera* SceneManager::GetCameraByHandle(SceneHandle sceneHandle, CameraHandle cameraHandle)
		{
			Scene* scene = GetSceneByHandle(sceneHandle);
			return scene->GetCameraByHandle(cameraHandle);
		}

		void SceneManager::SetActiveCamera(SceneHandle sceneHandle, CameraHandle camHandle)
		{
			Scene* scene = GetSceneByHandle(sceneHandle);
			scene->SetActiveCamera(camHandle);
		}

		Scene* SceneManager::GetSceneByHandle(SceneHandle handle)
		{
			if (!handle.IsValid() || handle.index >= m_sceneQueue.size())
			{
				throw std::runtime_error("Invalid mesh handle.");
			}

			return &m_sceneQueue[handle.index];
		}

		void SceneManager::RenderScene(SceneHandle handle)
		{
			Scene* scene = GetSceneByHandle(handle);
			scene->Render(m_renderer);
		}
	}
}