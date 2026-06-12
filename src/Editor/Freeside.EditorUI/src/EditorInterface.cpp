#include "..\include\EditorInterface.h"

namespace Freeside::Editor
{
	void EditorInterface::Initialize(Renderer* renderer)
	{
		m_renderer = renderer;
	}

	void EditorInterface::Draw(Scene::SceneManager& sceneManager)
	{
		Freeside::Scene::Scene* scene = sceneManager.GetActiveScene();

		if (!scene)
			return;

		DrawEntityHierarchy(*scene);
		DrawInspector(*scene);
	}

	void EditorInterface::DrawEntityHierarchy(Scene::Scene& scene)
	{
	}

	void EditorInterface::DrawInspector(Scene::Scene& scene)
	{
	}

	void EditorInterface::DrawAddComponentMenu(Scene::Scene& scene, Entity entity)
	{
	}
}