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
	Freeside::MeshData wallMeshData = Freeside::Shapes::Wall().mesh;

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
	Freeside::MeshHandle wallMeshHandle = renderer.CreateMesh(wallMeshData);

	Freeside::MaterialDesc earthMaterial;
	earthMaterial.baseColor = Freeside::Math::Vec3(0.0f, 0.0f, 1.0f);
	earthMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	Freeside::MaterialHandle earthMaterialHandle = renderer.RegisterMaterial(earthMaterial);

	Freeside::MaterialDesc crateMaterial;
	crateMaterial.baseColor = Freeside::Math::Vec3(1.0f, 0.0f, 0.0f);
	crateMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	crateMaterial.baseColorTexturePath = L"assets/textures/crate.png";
	Freeside::MaterialHandle crateMaterialHandle = renderer.RegisterMaterial(crateMaterial);


	Freeside::MaterialDesc pyramidMaterial;
	pyramidMaterial.baseColor = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f);
	pyramidMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	Freeside::MaterialHandle pyramidMaterialHandle = renderer.RegisterMaterial(pyramidMaterial);

	Freeside::MaterialDesc grassMaterial;
	grassMaterial.baseColor = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f);
	grassMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	grassMaterial.uvScale = Freeside::Math::Vec2(10.0f, 10.0f);
	Freeside::MaterialHandle grassMaterialHandle = renderer.RegisterMaterial(grassMaterial);

	Freeside::MaterialDesc wallMaterial;
	wallMaterial.baseColor = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f);
	wallMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	wallMaterial.uvScale = Freeside::Math::Vec2(2.0f, 2.0f);
	wallMaterial.baseColorTexturePath = L"assets/textures/brick.png";
	Freeside::MaterialHandle wallMaterialHandle = renderer.RegisterMaterial(wallMaterial);

	Freeside::MaterialDesc floorMaterial;
	floorMaterial.baseColor = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f);
	floorMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	floorMaterial.uvScale = Freeside::Math::Vec2(2.0f, 2.0f);
	floorMaterial.baseColorTexturePath = L"assets/textures/floor.png";
	Freeside::MaterialHandle floorMaterialHandle = renderer.RegisterMaterial(floorMaterial);

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

	Freeside::RenderObject box1;
	Freeside::RenderObject box2;
	Freeside::RenderObject box3;
	Freeside::RenderObject pyramid;
	Freeside::RenderObject sphere;
	Freeside::RenderObject wallBack;
	Freeside::RenderObject floor;
	Freeside::RenderObject wallLeft;
	Freeside::RenderObject wallRight;
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

	box1.mesh = cubeMeshHandle;
	box1.material = crateMaterialHandle;
	box1.transform.position = Freeside::Math::Vec3(-1.0f, 0.0f, 0.0f);
	box1.transform.rotation = Freeside::Math::Vec3(0.0f, Freeside::Math::PI * 0.1f, 0.0f);
	box1.transform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	box1.world = Freeside::Math::TransformMatrix(box1.transform.position, box1.transform.rotation, box1.transform.scale);
	box1.initialTransform = box1.world;
	box1.name = L"Cube";

	box2.mesh = cubeMeshHandle;
	box2.material = crateMaterialHandle;
	box2.transform.position = Freeside::Math::Vec3(1.0f, 0.0f, 0.0f);
	box2.transform.rotation = Freeside::Math::Vec3(0.0f, Freeside::Math::PI * 0.2f, 0.0f);
	box2.transform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	box2.world = Freeside::Math::TransformMatrix(box2.transform.position, box2.transform.rotation, box2.transform.scale);
	box2.initialTransform = box2.world;
	box2.name = L"Cube";

	box3.mesh = cubeMeshHandle;
	box3.material = crateMaterialHandle;
	box3.transform.position = Freeside::Math::Vec3(1.0f, 1.0f, 0.0f);
	box3.transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	box3.transform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	box3.world = Freeside::Math::TransformMatrix(box3.transform.position, box3.transform.rotation, box3.transform.scale);
	box3.initialTransform = box3.world;
	box3.name = L"Cube";

	pyramid.mesh = pyramidMeshHandle;
	pyramid.material = pyramidMaterialHandle;
	pyramid.transform.position = Freeside::Math::Vec3(1.0f, 0.0f, 0.0f);
	pyramid.transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	pyramid.transform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	pyramid.world = Freeside::Math::TransformMatrix(pyramid.transform.position, pyramid.transform.rotation, pyramid.transform.scale);
	pyramid.initialTransform = pyramid.world;
	pyramid.name = L"Pyramid";

	sphere.mesh = sphereMeshHandle;
	sphere.material = earthMaterialHandle;
	sphere.world = Freeside::Math::Translation(0.0f, 1.0f, 0.0f);
	sphere.initialTransform = Freeside::Math::Translation(0.0f, 1.0f, 0.0f);
	sphere.name = L"Sphere";

	wallBack.mesh = wallMeshHandle;
	wallBack.material = wallMaterialHandle;
	wallBack.transform.position = Freeside::Math::Vec3(0.0f, 2.0f, 2.5f);
	wallBack.transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	wallBack.transform.scale = Freeside::Math::Vec3(5.0f, 5.0f, 5.0f);
	wallBack.world = Freeside::Math::TransformMatrix(wallBack.transform.position, wallBack.transform.rotation, wallBack.transform.scale);
	wallBack.initialTransform = wallBack.world;
	wallBack.name =  L"Plane";

	wallLeft.mesh = wallMeshHandle;
	wallLeft.material = wallMaterialHandle;
	wallLeft.transform.position = Freeside::Math::Vec3(-2.5f, 2.0f, 0.0f);
	wallLeft.transform.rotation = Freeside::Math::Vec3(0.0f, -3.14159265f * 0.5f, 0.0f);
	wallLeft.transform.scale = Freeside::Math::Vec3(5.0f, 5.0f, 5.0f);
	wallLeft.world = Freeside::Math::TransformMatrix(wallLeft.transform.position, wallLeft.transform.rotation, wallLeft.transform.scale);
	wallLeft.initialTransform = wallLeft.world;
	wallLeft.name = L"Plane";

	wallRight.mesh = wallMeshHandle;
	wallRight.material = wallMaterialHandle;
	wallRight.transform.position = Freeside::Math::Vec3(2.5f, 2.0f, 0.0f);
	wallRight.transform.rotation = Freeside::Math::Vec3(0.0f, 3.14159265f * 0.5f, 0.0f);
	wallRight.transform.scale = Freeside::Math::Vec3(5.0f, 5.0f, 5.0f);
	wallRight.world = Freeside::Math::TransformMatrix(wallRight.transform.position, wallRight.transform.rotation, wallRight.transform.scale);
	wallRight.initialTransform = wallRight.world;
	wallRight.name = L"Plane";

	floor.mesh = planeMeshHandle;
	floor.material = floorMaterialHandle;
	floor.transform.position = Freeside::Math::Vec3(0.0f, -0.5f, 0.0f);
	floor.transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	floor.transform.scale = Freeside::Math::Vec3(5.0f, 5.0f, 5.0f);
	floor.world = Freeside::Math::TransformMatrix(floor.transform.position, floor.transform.rotation, floor.transform.scale);
	floor.initialTransform = floor.world;
	floor.name = L"Plane";


	hObject1 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, box1);
	hObject1 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, box2);
	hObject1 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, box3);
	//hObject2 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, pyramid);
	//hObject3 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, sphere);
	hObject4 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, wallBack);
	hObject4 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, wallLeft);
	hObject4 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, wallRight);
	hObject5 = sceneManager.AddRenderObjectToRenderQueue(testSceneHandle, floor);
	pObject1 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject1);
	/*
	pObject2 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject2);
	pObject3 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject3);
	pObject4 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject4);
	pObject5 = sceneManager.GetRenderObjectByHandle(testSceneHandle, hObject5);
	*/

	Freeside::DirectionalLightHandle hDirLight = Freeside::DirectionalLightHandle(0);
	Freeside::Lights::Directional* pDirLight = sceneManager.GetDirectionalLightByHandle(testSceneHandle, hDirLight);
	pDirLight->intensity = 0.0f;

	Freeside::Lights::Point pointLight1;
	pointLight1.color = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	pointLight1.intensity = 1.0f;
	pointLight1.position = Freeside::Math::Vec3(0.0f, 0.5f, -2.0f);
	pointLight1.radius = 20.0f;

	Freeside::Lights::Point pointLight2;
	pointLight2.color = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
	pointLight2.intensity = 1.0f;
	pointLight2.position = Freeside::Math::Vec3(0.0f, 0.5f, 1.5f);
	pointLight2.radius = 20.0f;

	Freeside::PointLightHandle hPointLight1 = sceneManager.AddPointLightToScene(testSceneHandle, pointLight1);
	Freeside::PointLightHandle hPointLight2 = sceneManager.AddPointLightToScene(testSceneHandle, pointLight2);
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

		//pPointLight1->color = HSVtoRGB(hue, 1.0f, 1.0f);

		/*
		Freeside::Math::Mat4 translation1 = box1.initialTransform;
		pObject1->world = translation1 * rotation;
        Freeside::Math::Mat4 translation2 = object2.initialTransform;
        pObject2->world = translation2 * rotation;
        Freeside::Math::Mat4 translation3 = object3.initialTransform;
        pObject3->world = translation3 * rotation;
		*/

		window.PollEvents();
		sceneManager.RenderScene(testSceneHandle);
	}
}

