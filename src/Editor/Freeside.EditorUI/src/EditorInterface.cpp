#include "..\include\EditorInterface.h"

namespace Freeside::Editor
{
	void EditorInterface::Initialize(Renderer* renderer)
	{
		m_renderer = renderer;
	}

	void EditorInterface::Update()
	{
		ImGui::ShowDemoWindow();
	}
}