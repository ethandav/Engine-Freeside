#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Platform\include\Window.h"
#include "..\..\Engine\efg.Renderer\include\Renderer.h"
#include "..\..\Engine\efg.Core\include\math\math.h"
#include "..\..\Engine\efg.Renderer\include\MeshData.h"
#include "..\..\Engine\efg.Renderer\include\Camera.h"
#include "..\..\Engine\efg.Renderer\include\SceneManager.h"

void Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	Window window;
	Renderer renderer;
	RendererDesc rendererDesc = {
		nullptr,
		1280,
		720
	};

	SceneManager scene;

	efg::MeshData triangleMeshData;
	efg::MeshData cubeMeshData;

	triangleMeshData.vertices =
	{
		{ efg::Vec3{  0.0f,  0.5f, 0.0f }, efg::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ efg::Vec3{  0.5f, -0.5f, 0.0f }, efg::Vec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ efg::Vec3{ -0.5f, -0.5f, 0.0f }, efg::Vec4{ 0.0f, 0.0f, 1.0f, 1.0f } },
	};

	triangleMeshData.indices =
	{
		0, 1, 2
	};

	cubeMeshData.vertices =
	{
		{ efg::Vec3{ -0.5f,  0.5f,  0.5f }, efg::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ efg::Vec3{  0.5f,  0.5f,  0.5f }, efg::Vec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ efg::Vec3{  0.5f, -0.5f,  0.5f }, efg::Vec4{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ efg::Vec3{ -0.5f, -0.5f,  0.5f }, efg::Vec4{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ efg::Vec3{ -0.5f,  0.5f, -0.5f }, efg::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ efg::Vec3{  0.5f,  0.5f, -0.5f }, efg::Vec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ efg::Vec3{  0.5f, -0.5f, -0.5f }, efg::Vec4{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ efg::Vec3{ -0.5f, -0.5f, -0.5f }, efg::Vec4{ 0.0f, 0.0f, 1.0f, 1.0f } },
	};

	cubeMeshData.indices =
	{
		0, 1, 2,
		0, 2, 3,
		4, 5, 1,
		4, 1, 0,
		5, 4, 7,
		5, 7, 6,
		4, 0, 3,
		4, 3, 7,
		1, 5, 6,
		1, 6, 2,
		3, 2, 6,
		3, 6, 7
	};

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);

	efg::Camera camera;
	camera.LookAt(efg::Vec3(0.0f, 1.0f, -5.0f), efg::Vec3(0.0f, 0.0f, 0.0f));
	camera.SetPerspective(0.78539816339f, static_cast<float>(rendererDesc.width) / static_cast<float>(rendererDesc.height), 0.1f, 1000.0f);

	efg::MeshHandle triangleMeshHandle = renderer.CreateMesh(triangleMeshData);
	efg::MeshHandle cubeMeshHandle = renderer.CreateMesh(cubeMeshData);

	RenderObject object;
	object.mesh = cubeMeshHandle;
	object.world = efg::Translation(-1.0f, 0.0f, 0.0f);
	scene.AddRenderObjectToScene(object);

	while (window.IsOpen())
	{
		window.PollEvents();
		renderer.BeginFrame(camera);
		//renderer.DrawMesh(triangleMeshHandle);
		renderer.DrawMesh(cubeMeshHandle);
		renderer.EndFrame();
	}
}

