#include "Scene.h"
#include <stdexcept>
namespace efg
{
	namespace Scene
	{
		Scene::Scene(std::wstring name) : name(name)
		{
		}

		void Scene::CreateScenefromDefault(float aspectRatio)
		{
			m_camera.LookAt(efg::Vec3(0.0f, 1.0f, -5.0f), efg::Vec3(0.0f, 0.0f, 0.0f));
			m_camera.SetPerspective(0.78539816339f, aspectRatio, 0.1f, 1000.0f);

			m_dirLight.color = Vec3(1.0f, 1.0f, 1.0f);
			m_dirLight.direction = Vec3(-0.2f, -1.0f, -0.3f);
		}

		SceneRenderObjectHandle Scene::AddRenderObjectToRenderQueue(RenderObject object)
		{
			m_renderObjectQueue.push_back(std::move(object));
			return SceneRenderObjectHandle
			{
				static_cast<uint32_t>(m_renderObjectQueue.size() - 1)
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

		void Scene::AddCamera(Camera camera)
		{
			m_camera = camera;
		}

		void Scene::Render(Renderer* renderer)
		{
			SceneRenderData renderData = {};
			renderData.camera = &m_camera;
			renderData.directionalLight = &m_dirLight;
			renderData.renderObjects = &m_renderObjectQueue;

			renderer->Render(renderData);
		}
	}
}
