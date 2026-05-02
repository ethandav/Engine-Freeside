#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Platform\include\Window.h"
#include "..\..\Engine\efg.Renderer\include\Renderer.h"

void Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	Window window;
	Renderer renderer;
	RendererDesc rendererDesc = {
		nullptr,
		1280,
		720
	};

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);

	while (window.IsOpen())
	{
		window.PollEvents();
		renderer.BeginFrame();
		renderer.Clear();
		renderer.EndFrame();
	}
}