#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Platform\include\Window.h"
#include "..\..\Engine\efg.Renderer\include\Renderer.h"
#include "..\..\Engine\efg.Core\include\math\math.h"
#include "..\..\Engine\efg.Renderer\include\MeshData.h"
#include "..\..\Engine\efg.Renderer\include\Camera.h"
#include "..\..\Engine\efg.Scene\include\SceneManager.h"

void Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	Window window;
	Renderer renderer;
	efg::Scene::SceneManager sceneManager;
	RendererDesc rendererDesc = {
		nullptr,
		1280,
		720
	};

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
	sceneManager.Initialize(&renderer);

	efg::Camera camera;
	camera.LookAt(efg::Vec3(0.0f, 1.0f, -5.0f), efg::Vec3(0.0f, 0.0f, 0.0f));
	camera.SetPerspective(0.78539816339f, static_cast<float>(rendererDesc.width) / static_cast<float>(rendererDesc.height), 0.1f, 1000.0f);

	efg::MeshHandle triangleMeshHandle = renderer.CreateMesh(triangleMeshData);
	efg::MeshHandle cubeMeshHandle = renderer.CreateMesh(cubeMeshData);

	RenderObject object1;
	RenderObject object2;
	RenderObject object3;
	object1.mesh = cubeMeshHandle;
	object1.world = efg::Translation(-1.0f, 0.0f, 0.0f);
	object2.mesh = cubeMeshHandle;
	object2.world = efg::Translation(1.0f, 0.0f, 0.0f);
	object3.mesh = cubeMeshHandle;
	object3.world = efg::Translation(0.0f, 1.0f, 0.0f);

	efg::Scene::SceneHandle testSceneHandle = sceneManager.CreateScene(L"Test Scene");
	sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object1);
	sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object2);
	sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object3);
	sceneManager.AddCamera(testSceneHandle, &camera);

	while (window.IsOpen())
	{
		window.PollEvents();
		sceneManager.RenderScene(testSceneHandle);
	}
}

