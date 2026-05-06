#include "Scene.h"
namespace efg
{
	namespace Scene
	{
		Scene::Scene(std::wstring name) : name(name)
		{
		}

		void Scene::AddRenderObjectToRenderQueue(RenderObject& object)
		{
			m_renderObjectQueue.push_back(&object);
		}

		void Scene::AddCamera(Camera* camera)
		{
			m_camera = camera;
		}

		void Scene::Render(Renderer* renderer)
		{
			renderer->BeginFrame(m_camera);
			renderer->EndFrame();
		}
	}
}
