#include "SceneManager.h"
#include <stdexcept>

namespace efg
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

		RenderObject* SceneManager::GetRenderObjectByHandle(SceneHandle sceneHandle, SceneRenderObjectHandle objectHandle)
		{
			Scene* scene = GetSceneByHandle(sceneHandle);
			return scene->GetRenderObjectByHandle(objectHandle);
		}

		void SceneManager::AddCamera(SceneHandle handle, Camera camera)
		{
			Scene* scene = GetSceneByHandle(handle);
			scene->AddCamera(camera);
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