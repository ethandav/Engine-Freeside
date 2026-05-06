#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Platform\include\Window.h"
#include "..\..\Engine\efg.Platform\include\Timer.h"
#include "..\..\Engine\efg.Renderer\include\Renderer.h"
#include "..\..\Engine\efg.Core\include\math\math.h"
#include "..\..\Engine\efg.Core\include\shapes\shapes.h"
#include "..\..\Engine\efg.Renderer\include\MeshData.h"
#include "..\..\Engine\efg.Renderer\include\Camera.h"
#include "..\..\Engine\efg.Scene\include\SceneManager.h"

void Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	Window window;
	Timer timer;
	Renderer renderer;
	efg::Scene::SceneManager sceneManager;
	RendererDesc rendererDesc = {
		nullptr,
		1280,
		720
	};

	efg::MeshData cubeMeshData = efg::Shapes::Cube().mesh;

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);
	sceneManager.Initialize(&renderer);

	efg::Camera camera;
	camera.LookAt(efg::Vec3(0.0f, 1.0f, -5.0f), efg::Vec3(0.0f, 0.0f, 0.0f));
	camera.SetPerspective(0.78539816339f, static_cast<float>(rendererDesc.width) / static_cast<float>(rendererDesc.height), 0.1f, 1000.0f);

	efg::MeshHandle cubeMeshHandle = renderer.CreateMesh(cubeMeshData);

	RenderObject object1;
	RenderObject object2;
	RenderObject object3;
	object1.mesh = cubeMeshHandle;
	object1.world = efg::Translation(-1.0f, 0.0f, 0.0f);
	object1.initialTransform = efg::Translation(-1.0f, 0.0f, 0.0f);
	object2.mesh = cubeMeshHandle;
	object2.world = efg::Translation(1.0f, 0.0f, 0.0f);
	object2.initialTransform = efg::Translation(1.0f, 0.0f, 0.0f);
	object3.mesh = cubeMeshHandle;
	object3.world = efg::Translation(0.0f, 1.0f, 0.0f);
	object3.initialTransform = efg::Translation(0.0f, 1.0f, 0.0f);

	efg::Scene::SceneHandle testSceneHandle = sceneManager.CreateScene(L"Test Scene");
	sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object1);
	sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object2);
	sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object3);
	sceneManager.AddCamera(testSceneHandle, &camera);

	float angle = 0.0f;
	while (window.IsOpen())
	{
		const float deltaTime = timer.Tick();
		angle += 1.0f * deltaTime;
		efg::Mat4 rotation = efg::RotationY(angle);

		efg::Mat4 translation1 = object1.initialTransform;
		object1.world = translation1 * rotation;
        efg::Mat4 translation2 = object2.initialTransform;
        object2.world = translation2 * rotation;
        efg::Mat4 translation3 = object3.initialTransform;
        object3.world = translation3 * rotation;

		window.PollEvents();
		sceneManager.RenderScene(testSceneHandle);
	}
}

