#pragma once
#include "..\..\..\Engine\efg\include\render\Renderer.h"
#include "..\..\..\Editor\Freeside.EditorUI\imgui\imgui.h"

namespace Freeside::Editor
{
	class EditorInterface
	{
	public:
		void Initialize(Renderer* renderer);
		void Update();
	private:
		Renderer* m_renderer;
	};
}