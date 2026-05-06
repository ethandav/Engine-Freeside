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

		void SceneManager::AddRenderObjectToRenderQueue(SceneHandle handle, RenderObject& object)
		{
			Scene* scene = GetSceneByHandle(handle);
			scene->AddRenderObjectToRenderQueue(object);
			m_renderer->AddRenderObjectToRenderQueue(object);
		}

		void SceneManager::AddCamera(SceneHandle handle, Camera* camera)
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