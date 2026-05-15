#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Renderer\efg.h"

#include <random>

constexpr uint32_t ObjectCount = 10000;

void Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	Window window;
	Timer timer;
	Freeside::Renderer renderer;
	Freeside::Scene::SceneManager sceneManager;
	Freeside::RendererDesc rendererDesc = {
		nullptr,
		1920,
		1080
	};

	Freeside::MeshData cubeMeshData = Freeside::Shapes::Cube().mesh;
	Freeside::MeshData sphereMeshData = Freeside::Shapes::Sphere().mesh;
	Freeside::MeshData pyramidMeshData = Freeside::Shapes::Pyramid().mesh;

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);
	sceneManager.Initialize(&renderer);
	Freeside::Scene::SceneHandle testSceneHandle = sceneManager.CreateScene(L"Test Scene");

	Freeside::MeshHandle cubeMeshHandle = renderer.CreateMesh(cubeMeshData);
	Freeside::MeshHandle sphereMeshHandle = renderer.CreateMesh(sphereMeshData);
	Freeside::MeshHandle pyramidMeshHandle = renderer.CreateMesh(pyramidMeshData);
	
	Freeside::TextureHandle earthTexture = renderer.RegisterTexture2D(L"assets/textures/earth.jpeg");
	Freeside::TextureHandle crateTexture = renderer.RegisterTexture2D(L"assets/textures/crate.png");
	Freeside::TextureHandle pyramidTexture = renderer.RegisterTexture2D(L"assets/textures/pyramid.jpg");

	Freeside::MaterialDesc earthMaterial;
	earthMaterial.baseColor = Freeside::Math::Vec3(0.0f, 0.0f, 1.0f);
	earthMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	earthMaterial.baseColorTexture2D = earthTexture;
	Freeside::MaterialHandle earthMaterialHandle = renderer.RegisterMaterial(earthMaterial);


	Freeside::MaterialDesc crateMaterial;
	crateMaterial.baseColor = Freeside::Math::Vec3(1.0f, 0.0f, 0.0f);
	crateMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	crateMaterial.baseColorTexture2D = crateTexture;
	Freeside::MaterialHandle crateMaterialHandle = renderer.RegisterMaterial(crateMaterial);


	Freeside::MaterialDesc pyramidMaterial;
	pyramidMaterial.baseColor = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f);
	pyramidMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	pyramidMaterial.baseColorTexture2D = pyramidTexture;
	Freeside::MaterialHandle pyramidMaterialHandle = renderer.RegisterMaterial(pyramidMaterial);

	std::mt19937 rng{ std::random_device{}() };

	std::uniform_real_distribution<float> posDist(-50.0f, 50.0f);
	std::uniform_real_distribution<float> heightDist(-50.0f, 50.0f);

	/*
	for (uint32_t i = 0; i < ObjectCount; ++i)
	{
		efg::RenderObject object = {};

		const float x = posDist(rng);
		const float y = heightDist(rng);
		const float z = posDist(rng);

		Freeside::Math::Mat4 transform = Freeside::Math::Translation(x, y, z);

		object.mesh = sphereMeshHandle;
		object.material = earthMaterialHandle;
		object.world = transform;
		object.initialTransform = transform;
		sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object);
	}
	*/

	Freeside::RenderObject object1;
	Freeside::RenderObject object2;
	Freeside::RenderObject object3;
	Freeside::RenderObject* pObject1;
	Freeside::RenderObject* pObject2;
	Freeside::RenderObject* pObject3;
	Freeside::Scene::SceneRenderObjectHandle hObject1;
	Freeside::Scene::SceneRenderObjectHandle hObject2;
	Freeside::Scene::SceneRenderObjectHandle hObject3;
	object1.mesh = cubeMeshHandle;
	object1.material = crateMaterialHandle;
	object1.world = Freeside::Math::Translation(-1.0f, 0.0f, 0.0f);
	object1.initialTransform = Freeside::Math::Translation(-1.0f, 0.0f, 0.0f);
	object1.name = L"Cube";
	object2.mesh = pyramidMeshHandle;
	object2.material = pyramidMaterialHandle;
	object2.world = Freeside::Math::Translation(1.0f, 0.0f, 0.0f);
	object2.initialTransform = Freeside::Math::Translation(1.0f, 0.0f, 0.0f);
	object2.name = L"Pyramid";
	object3.mesh = sphereMeshHandle;
	object3.material = earthMaterialHandle;
	object3.world = Freeside::Math::Translation(0.0f, 1.0f, 0.0f);
	object3.initialTransform = Freeside::Math::Translation(0.0f, 1.0f, 0.0f);
	object3.name = L"Sphere";


	hObject1 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object1);
	hObject2 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object2);
	hObject3 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object3);
	pObject1 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject1);
	pObject2 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject2);
	pObject3 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject3);


	Freeside::Lights::Point pointLight1;
	pointLight1.color = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	pointLight1.intensity = 1.0f;
	pointLight1.position = Freeside::Math::Vec3(-1.0f, 0.0f, -2.0f);
	pointLight1.radius = 5.0f;

	sceneManager.AddPointLightToScene(testSceneHandle, pointLight1);

	float angle = 0.0f;
	while (window.IsOpen())
	{
		const float deltaTime = timer.Tick();
		angle += 1.0f * deltaTime;
		Freeside::Math::Mat4 rotation = Freeside::Math::RotationY(angle);

		Freeside::Math::Mat4 translation1 = object1.initialTransform;
		pObject1->world = translation1 * rotation;
        Freeside::Math::Mat4 translation2 = object2.initialTransform;
        pObject2->world = translation2 * rotation;
        Freeside::Math::Mat4 translation3 = object3.initialTransform;
        pObject3->world = translation3 * rotation;

		window.PollEvents();
		sceneManager.RenderScene(testSceneHandle);
	}
}

