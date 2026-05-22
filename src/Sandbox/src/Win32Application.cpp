#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Renderer\efg.h"

#include <random>

constexpr uint32_t ObjectCount = 10000;

Freeside::Math::Vec3 HSVtoRGB(float h, float s, float v)
{
	float r = 0, g = 0, b = 0;

	float i = floorf(h * 6.0f);
	float f = h * 6.0f - i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - f * s);
	float t = v * (1.0f - (1.0f - f) * s);

	switch (static_cast<int>(i) % 6)
	{
	case 0: r = v; g = t; b = p; break;
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	}

	return Freeside::Math::Vec3{ r, g, b };
}

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
	Freeside::MeshData planeMeshData = Freeside::Shapes::Plane().mesh;

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);
	sceneManager.Initialize(&renderer);
	Freeside::Scene::SceneHandle testSceneHandle = sceneManager.CreateScene(L"Test Scene");

	Freeside::MeshHandle cubeMeshHandle = renderer.CreateMesh(cubeMeshData);
	Freeside::MeshHandle sphereMeshHandle = renderer.CreateMesh(sphereMeshData);
	Freeside::MeshHandle pyramidMeshHandle = renderer.CreateMesh(pyramidMeshData);
	Freeside::MeshHandle planeMeshHandle = renderer.CreateMesh(planeMeshData);
	
	Freeside::TextureHandle earthTexture = renderer.RegisterTexture2D(L"assets/textures/earth.jpeg");
	Freeside::TextureHandle crateTexture = renderer.RegisterTexture2D(L"assets/textures/crate.png");
	Freeside::TextureHandle pyramidTexture = renderer.RegisterTexture2D(L"assets/textures/pyramid.jpg");
	Freeside::TextureHandle grassTexture = renderer.RegisterTexture2D(L"assets/textures/grass.jpg");

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

	Freeside::MaterialDesc grassMaterial;
	grassMaterial.baseColor = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f);
	grassMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	grassMaterial.uvScale = Freeside::Math::Vec2(10.0f, 10.0f);
	grassMaterial.baseColorTexture2D = grassTexture;
	Freeside::MaterialHandle grassMaterialHandle = renderer.RegisterMaterial(grassMaterial);

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
	Freeside::RenderObject object4;
	Freeside::RenderObject object5;
	Freeside::RenderObject* pObject1;
	Freeside::RenderObject* pObject2;
	Freeside::RenderObject* pObject3;
	Freeside::RenderObject* pObject4;
	Freeside::RenderObject* pObject5;
	Freeside::Scene::SceneRenderObjectHandle hObject1;
	Freeside::Scene::SceneRenderObjectHandle hObject2;
	Freeside::Scene::SceneRenderObjectHandle hObject3;
	Freeside::Scene::SceneRenderObjectHandle hObject4;
	Freeside::Scene::SceneRenderObjectHandle hObject5;
	object1.mesh = cubeMeshHandle;
	object1.material = crateMaterialHandle;
	object1.transform.position = Freeside::Math::Vec3(-1.0f, 0.0f, 0.0f);
	object1.transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	object1.transform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	object1.world = Freeside::Math::TransformMatrix(object1.transform.position, object1.transform.rotation, object1.transform.scale);
	object1.initialTransform = object1.world;
	object1.name = L"Cube";

	object2.mesh = pyramidMeshHandle;
	object2.material = pyramidMaterialHandle;
	object2.transform.position = Freeside::Math::Vec3(1.0f, 0.0f, 0.0f);
	object2.transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	object2.transform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	object2.world = Freeside::Math::TransformMatrix(object2.transform.position, object2.transform.rotation, object2.transform.scale);
	object2.initialTransform = object2.world;
	object2.name = L"Pyramid";

	object3.mesh = sphereMeshHandle;
	object3.material = earthMaterialHandle;
	object3.world = Freeside::Math::Translation(0.0f, 1.0f, 0.0f);
	object3.initialTransform = Freeside::Math::Translation(0.0f, 1.0f, 0.0f);
	object3.name = L"Sphere";

	object4.mesh = planeMeshHandle;
	object4.material = grassMaterialHandle;
	object4.transform.position = Freeside::Math::Vec3(0.0f, 0.0f, 5.0f);
	object4.transform.rotation = Freeside::Math::Vec3(-3.14159265f * 0.5f, 0.0f, 0.0f);
	object4.transform.scale = Freeside::Math::Vec3(10.0f, 10.0f, 10.0f);
	object4.world = Freeside::Math::TransformMatrix(object4.transform.position, object4.transform.rotation, object4.transform.scale);
	object4.initialTransform = object4.world;
	object4.name =  L"Plane";

	object5.mesh = planeMeshHandle;
	object5.material = grassMaterialHandle;
	object5.transform.position = Freeside::Math::Vec3(0.0f, -0.5f, 0.0f);
	object5.transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	object5.transform.scale = Freeside::Math::Vec3(10.0f, 10.0f, 10.0f);
	object5.world = Freeside::Math::TransformMatrix(object5.transform.position, object5.transform.rotation, object5.transform.scale);
	object5.initialTransform = object5.world;
	object5.name = L"Plane";


	hObject1 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object1);
	hObject2 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object2);
	hObject3 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object3);
	hObject4 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object4);
	hObject5 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, object5);
	pObject1 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject1);
	pObject2 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject2);
	pObject3 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject3);
	pObject4 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject4);
	pObject5 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject5);

	Freeside::DirectionalLightHandle hDirLight = Freeside::DirectionalLightHandle(0);
	Freeside::Lights::Directional* pDirLight = sceneManager.GetDirectionalLightByHandle(testSceneHandle, hDirLight);
	pDirLight->intensity = 0.0f;

	Freeside::Lights::Point pointLight1;
	pointLight1.color = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	pointLight1.intensity = 1.0f;
	pointLight1.position = Freeside::Math::Vec3(-1.0f, 0.0f, -3.0f);
	pointLight1.radius = 20.0f;

	Freeside::PointLightHandle hPointLight1 = sceneManager.AddPointLightToScene(testSceneHandle, pointLight1);
	Freeside::Lights::Point* pPointLight1 = sceneManager.GetPointLightByHandle(testSceneHandle, hPointLight1);

	float angle = 0.0f;
	float speed = 1.0f;
	float daySpeed = 0.1f;
	float totalTime = 0.0f;
	while (window.IsOpen())
	{
		const float deltaTime = timer.Tick();
		angle += 1.0f * deltaTime;
		totalTime += deltaTime;

		float hue = fmodf(totalTime * speed, 1.0f);

		Freeside::Math::Mat4 rotation = Freeside::Math::RotationY(angle);

		float t = fmodf(totalTime * daySpeed, 1.0f); // 0..1
		float angle = t * Freeside::Math::PI;                        // sunrise to sunset

		Freeside::Math::Vec3 sunPosition =
		{
			cosf(angle) * 50.0f,  // east-west
			sinf(angle) * 50.0f,  // height
			-20.0f                // slight south/north offset
		};

		pDirLight->direction = Freeside::Math::Normalize(Freeside::Math::Vec3{ 0,0,0 } - sunPosition);

		pPointLight1->color = HSVtoRGB(hue, 1.0f, 1.0f);

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

