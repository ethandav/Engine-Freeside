#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Renderer\efg.h"

#include <random>

constexpr uint32_t ObjectCount = 10000;

void Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	Window window;
	Timer timer;
	efg::Renderer renderer;
	efg::Scene::SceneManager sceneManager;
	efg::RendererDesc rendererDesc = {
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
	efg::Scene::SceneHandle testSceneHandle = sceneManager.CreateScene(L"Test Scene");

	efg::MeshHandle cubeMeshHandle = renderer.CreateMesh(cubeMeshData);
	efg::MeshHandle sphereMeshHandle = renderer.CreateMesh(sphereMeshData);
	efg::MeshHandle pyramidMeshHandle = renderer.CreateMesh(pyramidMeshData);

	efg::MaterialDesc blueMaterial;
	blueMaterial.baseColor = efg::Math::Vec3(0.0f, 0.0f, 1.0f);
	blueMaterial.specular = efg::Math::Vec2(1.0f, 64.0f);
	efg::MaterialHandle blueMaterialHandle = renderer.RegisterMaterial(blueMaterial);

	efg::MaterialDesc redMaterial;
	redMaterial.baseColor = efg::Math::Vec3(1.0f, 0.0f, 0.0f);
	redMaterial.specular = efg::Math::Vec2(1.0f, 64.0f);
	efg::MaterialHandle redMaterialHandle = renderer.RegisterMaterial(redMaterial);

	efg::MaterialDesc greenMaterial;
	greenMaterial.baseColor = efg::Math::Vec3(0.0f, 1.0f, 0.0f);
	greenMaterial.specular = efg::Math::Vec2(1.0f, 64.0f);
	efg::MaterialHandle greenMaterialHandle = renderer.RegisterMaterial(greenMaterial);


	std::mt19937 rng{ std::random_device{}() };

	std::uniform_real_distribution<float> posDist(-50.0f, 50.0f);
	std::uniform_real_distribution<float> heightDist(-50.0f, 50.0f);

	for (uint32_t i = 0; i < ObjectCount; ++i)
	{
		efg::RenderObject object = {};

		const float x = posDist(rng);
		const float y = heightDist(rng);
		const float z = posDist(rng);

		efg::Math::Mat4 transform = efg::Math::Translation(x, y, z);

		object.mesh = sphereMeshHandle;
		object.material = blueMaterialHandle;
		object.world = transform;
		object.initialTransform = transform;
		sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object);
	}

	efg::RenderObject object1;
	efg::RenderObject object2;
	efg::RenderObject object3;
	efg::RenderObject* pObject1;
	efg::RenderObject* pObject2;
	efg::RenderObject* pObject3;
	efg::Scene::SceneRenderObjectHandle hObject1;
	efg::Scene::SceneRenderObjectHandle hObject2;
	efg::Scene::SceneRenderObjectHandle hObject3;
	object1.mesh = cubeMeshHandle;
	object1.material = blueMaterialHandle;
	object1.world = efg::Math::Translation(-1.0f, 0.0f, 0.0f);
	object1.initialTransform = efg::Math::Translation(-1.0f, 0.0f, 0.0f);
	object1.name = L"Cube";
	object2.mesh = pyramidMeshHandle;
	object2.material = redMaterialHandle;
	object2.world = efg::Math::Translation(1.0f, 0.0f, 0.0f);
	object2.initialTransform = efg::Math::Translation(1.0f, 0.0f, 0.0f);
	object2.name = L"Pyramid";
	object3.mesh = sphereMeshHandle;
	object3.material = greenMaterialHandle;
	object3.world = efg::Math::Translation(0.0f, 1.0f, 0.0f);
	object3.initialTransform = efg::Math::Translation(0.0f, 1.0f, 0.0f);
	object3.name = L"Sphere";


	hObject1 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object1);
	hObject2 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object2);
	hObject3 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object3);
	pObject1 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject1);
	pObject2 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject2);
	pObject3 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject3);


	efg::Lights::Point pointLight1;
	pointLight1.color = efg::Math::Vec3(1.0f, 1.0f, 1.0f);
	pointLight1.intensity = 1.0f;
	pointLight1.position = efg::Math::Vec3(-1.0f, 0.0f, -2.0f);
	pointLight1.radius = 5.0f;

	sceneManager.AddPointLightToScene(testSceneHandle, pointLight1);

	float angle = 0.0f;
	while (window.IsOpen())
	{
		const float deltaTime = timer.Tick();
		angle += 1.0f * deltaTime;
		efg::Math::Mat4 rotation = efg::Math::RotationY(angle);

		efg::Math::Mat4 translation1 = object1.initialTransform;
		pObject1->world = translation1 * rotation;
        efg::Math::Mat4 translation2 = object2.initialTransform;
        pObject2->world = translation2 * rotation;
        efg::Math::Mat4 translation3 = object3.initialTransform;
        pObject3->world = translation3 * rotation;

		window.PollEvents();
		sceneManager.RenderScene(testSceneHandle);
	}
}

