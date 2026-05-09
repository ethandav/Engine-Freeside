#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Platform\include\Window.h"
#include "..\..\Engine\efg.Platform\include\Timer.h"
#include "..\..\Engine\efg.Renderer\include\Renderer.h"
#include "..\..\Engine\efg.Core\include\math\math.h"
#include "..\..\Engine\efg.Core\include\shapes\shapes.h"
#include "..\..\Engine\efg.Renderer\include\MeshData.h"
#include "..\..\Engine\efg.Renderer\include\Camera.h"
#include "..\..\Engine\efg.Scene\include\SceneManager.h"
#include "..\..\Engine\efg.Renderer\include\Materials.h"

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
	efg::MeshData sphereMeshData = efg::Shapes::Sphere().mesh;
	efg::MeshData pyramidMeshData = efg::Shapes::Pyramid().mesh;

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);
	sceneManager.Initialize(&renderer);

	efg::MeshHandle cubeMeshHandle = renderer.CreateMesh(cubeMeshData);
	efg::MeshHandle sphereMeshHandle = renderer.CreateMesh(sphereMeshData);
	efg::MeshHandle pyramidMeshHandle = renderer.CreateMesh(pyramidMeshData);

	efg::MaterialDesc blueMaterial;
	blueMaterial.baseColor = efg::Vec3(0.0f, 0.0f, 1.0f);
	blueMaterial.specular = efg::Vec2(1.0f, 64.0f);
	efg::MaterialHandle blueMaterialHandle = renderer.RegisterMaterial(blueMaterial);

	efg::MaterialDesc redMaterial;
	redMaterial.baseColor = efg::Vec3(1.0f, 0.0f, 0.0f);
	redMaterial.specular = efg::Vec2(1.0f, 64.0f);
	efg::MaterialHandle redMaterialHandle = renderer.RegisterMaterial(redMaterial);

	efg::MaterialDesc greenMaterial;
	greenMaterial.baseColor = efg::Vec3(0.0f, 1.0f, 0.0f);
	greenMaterial.specular = efg::Vec2(1.0f, 64.0f);
	efg::MaterialHandle greenMaterialHandle = renderer.RegisterMaterial(greenMaterial);

	RenderObject object1;
	RenderObject object2;
	RenderObject object3;
	RenderObject* pObject1;
	RenderObject* pObject2;
	RenderObject* pObject3;
	efg::Scene::SceneRenderObjectHandle hObject1;
	efg::Scene::SceneRenderObjectHandle hObject2;
	efg::Scene::SceneRenderObjectHandle hObject3;
	object1.mesh = cubeMeshHandle;
	object1.material = blueMaterialHandle;
	object1.world = efg::Translation(-1.0f, 0.0f, 0.0f);
	object1.initialTransform = efg::Translation(-1.0f, 0.0f, 0.0f);
	object1.name = L"Cube";
	object2.mesh = pyramidMeshHandle;
	object2.material = redMaterialHandle;
	object2.world = efg::Translation(1.0f, 0.0f, 0.0f);
	object2.initialTransform = efg::Translation(1.0f, 0.0f, 0.0f);
	object2.name = L"Pyramid";
	object3.mesh = sphereMeshHandle;
	object3.material = greenMaterialHandle;
	object3.world = efg::Translation(0.0f, 1.0f, 0.0f);
	object3.initialTransform = efg::Translation(0.0f, 1.0f, 0.0f);
	object3.name = L"Sphere";

	efg::Scene::SceneHandle testSceneHandle = sceneManager.CreateScene(L"Test Scene");
	hObject1 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object1);
	hObject2 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object2);
	hObject3 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object3);
	pObject1 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject1);
	pObject2 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject2);
	pObject3 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject3);

	efg::Lights::Point pointLight1;
	pointLight1.color = efg::Vec3(1.0f, 1.0f, 1.0f);
	pointLight1.intensity = 1.0f;
	pointLight1.position = efg::Vec3(-1.0f, 0.0f, -2.0f);
	pointLight1.radius = 5.0f;

	sceneManager.AddPointLightToScene(testSceneHandle, pointLight1);

	float angle = 0.0f;
	while (window.IsOpen())
	{
		const float deltaTime = timer.Tick();
		angle += 1.0f * deltaTime;
		efg::Mat4 rotation = efg::RotationY(angle);

		efg::Mat4 translation1 = object1.initialTransform;
		pObject1->world = translation1 * rotation;
        efg::Mat4 translation2 = object2.initialTransform;
        pObject2->world = translation2 * rotation;
        efg::Mat4 translation3 = object3.initialTransform;
        pObject3->world = translation3 * rotation;

		window.PollEvents();
		sceneManager.RenderScene(testSceneHandle);
	}
}

