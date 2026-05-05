#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Platform\include\Window.h"
#include "..\..\Engine\efg.Renderer\include\Renderer.h"
#include "..\..\Engine\efg.Core\include\math\math.h"
#include "..\..\Engine\efg.Renderer\include\MeshData.h"

void Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	Window window;
	Renderer renderer;
	RendererDesc rendererDesc = {
		nullptr,
		1280,
		720
	};

	Engine::MeshData triangleMeshData;

	triangleMeshData.vertices =
	{
		{ Engine::Vec3{  0.0f,  0.5f, 0.0f }, Engine::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ Engine::Vec3{  0.5f, -0.5f, 0.0f }, Engine::Vec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ Engine::Vec3{ -0.5f, -0.5f, 0.0f }, Engine::Vec4{ 0.0f, 0.0f, 1.0f, 1.0f } },
	};

	triangleMeshData.indices =
	{
		0, 1, 2
	};

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);
	Engine::MeshHandle triangleMeshHandle = renderer.CreateMesh(triangleMeshData);

	while (window.IsOpen())
	{
		window.PollEvents();
		renderer.BeginFrame();
		renderer.DrawMesh(triangleMeshHandle);
		renderer.EndFrame();
	}
}

