#include "..\include\EditorInterface.h"

namespace Freeside::Editor
{
	void EditorInterface::Initialize(Renderer* renderer)
	{
		m_renderer = renderer;
	}

	void EditorInterface::Draw(Scene::SceneManager& sceneManager)
	{
		ImGui::ShowDemoWindow();
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