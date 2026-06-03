#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Core\freeside.h"
#include "..\..\Engine\efg.Core\include\controllers\FirstPersonCameraController.h"

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
	Freeside::Window window;
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
	window.SetCursorLocked(true);

	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);
	float aspectRatio = renderer.GetRendererAspectRatio();
	sceneManager.Initialize(&renderer);

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
	crateMaterial.normalTexturePath = L"assets/textures/crate_normals.png";
	crateMaterial.heightTexturePath = L"assets/textures/crate_height.png";
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
	wallMaterial.normalTexturePath = L"assets/textures/brick_normals.png";
	wallMaterial.heightTexturePath = L"assets/textures/brick_height.png";
	Freeside::MaterialHandle wallMaterialHandle = renderer.RegisterMaterial(wallMaterial);

	Freeside::MaterialDesc floorMaterial;
	floorMaterial.baseColor = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f);
	floorMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	floorMaterial.uvScale = Freeside::Math::Vec2(2.0f, 2.0f);
	floorMaterial.baseColorTexturePath = L"assets/textures/floor.png";
	floorMaterial.normalTexturePath = L"assets/textures/floor_normals.png";
	floorMaterial.heightTexturePath = L"assets/textures/floor_height.png";
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



	Freeside::Scene::Scene testScene(L"Test Scene");

	Freeside::Entity eCamera = testScene.CreateEntity();
	Freeside::CameraComponent& cCamera = testScene.AddCamera(eCamera);
	cCamera.camera.SetPosition(Freeside::Math::Vec3(0.0f, 1.0f, -5.0f));
	cCamera.isMainCamera = true;

	Freeside::FirstPersonCameraController cameraController;
	cameraController.InitializeFromCamera(cCamera.camera);
	cameraController.SetMoveSpeed(5.0f);
	cameraController.SetMouseSensitivity(0.002f);

	Freeside::Entity eCrate = testScene.CreateEntity();
	Freeside::MeshRendererComponent& cCrateRenderer = testScene.AddMeshRenderer(eCrate);
	Freeside::TransformComponent& cCrateTransform = testScene.AddTransform(eCrate);
	cCrateRenderer.material = crateMaterialHandle;
	cCrateRenderer.mesh = cubeMeshHandle;
	cCrateTransform.position = Freeside::Math::Vec3(-1.0f, 0.0f, 0.0f);
	cCrateTransform.rotation = Freeside::Math::Vec3(0.0f, Freeside::Math::PI * 0.1f, 0.0f);
	cCrateTransform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);

	Freeside::Entity eCrate2 = testScene.CreateEntity();
	Freeside::MeshRendererComponent& cCrate2Renderer = testScene.AddMeshRenderer(eCrate2);
	Freeside::TransformComponent& cCrate2Transform = testScene.AddTransform(eCrate2);
	cCrate2Renderer.material = crateMaterialHandle;
	cCrate2Renderer.mesh = cubeMeshHandle;
	cCrate2Transform.position = Freeside::Math::Vec3(1.0f, 0.0f, 0.0f);
	cCrate2Transform.rotation = Freeside::Math::Vec3(0.0f, Freeside::Math::PI * 0.2f, 0.0f);
	cCrate2Transform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);

	Freeside::Entity eCrate3 = testScene.CreateEntity();
	Freeside::MeshRendererComponent& cCrate3Renderer = testScene.AddMeshRenderer(eCrate3);
	Freeside::TransformComponent& cCrate3Transform = testScene.AddTransform(eCrate3);
	cCrate3Renderer.material = crateMaterialHandle;
	cCrate3Renderer.mesh = cubeMeshHandle;
	cCrate3Transform.position = Freeside::Math::Vec3(1.0f, 1.0f, 0.0f);
	cCrate3Transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	cCrate3Transform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);

	Freeside::Entity ePointLight = testScene.CreateEntity();
	Freeside::PointLightComponent& cPointLight = testScene.AddPointLight(ePointLight);
	Freeside::TransformComponent& cPointLightTransform = testScene.AddTransform(ePointLight);
	cPointLight.intensity = 1.0f;
	cPointLightTransform.position = Freeside::Math::Vec3(-1.0f, 0.5f, -1.0f);

	Freeside::Entity eWall1 = testScene.CreateEntity();
	Freeside::MeshRendererComponent& cWall1Renderer = testScene.AddMeshRenderer(eWall1);
	Freeside::TransformComponent& cWall1Transform = testScene.AddTransform(eWall1);
	cWall1Renderer.material = wallMaterialHandle;
	cWall1Renderer.mesh = wallMeshHandle;
	cWall1Transform.position = Freeside::Math::Vec3(0.0f, 2.0f, 2.5f);
	cWall1Transform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	cWall1Transform.scale = Freeside::Math::Vec3(5.0f, 5.0f, 5.0f);

	Freeside::Entity eWall2 = testScene.CreateEntity();
	Freeside::MeshRendererComponent& cWall2Renderer = testScene.AddMeshRenderer(eWall2);
	Freeside::TransformComponent& cWall2Transform = testScene.AddTransform(eWall2);
	cWall2Renderer.material = wallMaterialHandle;
	cWall2Renderer.mesh = wallMeshHandle;
	cWall2Transform.position = Freeside::Math::Vec3(-2.5f, 2.0f, 0.0f);;
	cWall2Transform.rotation = Freeside::Math::Vec3(0.0f, -3.14159265f * 0.5f, 0.0f);
	cWall2Transform.scale = Freeside::Math::Vec3(5.0f, 5.0f, 5.0f);

	Freeside::Entity eWall3 = testScene.CreateEntity();
	Freeside::MeshRendererComponent& cWall3Renderer = testScene.AddMeshRenderer(eWall3);
	Freeside::TransformComponent& cWall3Transform = testScene.AddTransform(eWall3);
	cWall3Renderer.material = wallMaterialHandle;
	cWall3Renderer.mesh = wallMeshHandle;
	cWall3Transform.position = Freeside::Math::Vec3(2.5f, 2.0f, 0.0f);
	cWall3Transform.rotation = Freeside::Math::Vec3(0.0f, 3.14159265f * 0.5f, 0.0f);
	cWall3Transform.scale = Freeside::Math::Vec3(5.0f, 5.0f, 5.0f);

	Freeside::Entity eFloor = testScene.CreateEntity();
	Freeside::MeshRendererComponent& cFloorRenderer = testScene.AddMeshRenderer(eFloor);
	Freeside::TransformComponent& cFloorTransform = testScene.AddTransform(eFloor);
	cFloorRenderer.material = floorMaterialHandle;
	cFloorRenderer.mesh = planeMeshHandle;
	cFloorTransform.position = Freeside::Math::Vec3(0.0f, -0.5f, 0.0f);
	cFloorTransform.rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
	cFloorTransform.scale = Freeside::Math::Vec3(5.0f, 5.0f, 5.0f);

	float angle = 0.0f;
	float speed = 1.0f;
	float daySpeed = 0.1f;
	float totalTime = 0.0f;
	uint64_t frameId = 0;
	while (window.IsOpen())
	{
		frameId++;
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

		//pDirLight->direction = Freeside::Math::Normalize(Freeside::Math::Vec3{ 0,0,0 } - sunPosition);

		//pPointLight1->color = HSVtoRGB(hue, 1.0f, 1.0f);

		/*
		Freeside::Math::Mat4 translation1 = box1.initialTransform;
		pObject1->world = translation1 * rotation;
        Freeside::Math::Mat4 translation2 = object2.initialTransform;
        pObject2->world = translation2 * rotation;
        Freeside::Math::Mat4 translation3 = object3.initialTransform;
        pObject3->world = translation3 * rotation;
		*/

		Freeside::InputState input = window.PollInput();

		cameraController.Update(&cCamera.camera, input, deltaTime);

		sceneManager.RenderScene(testScene, frameId);
	}
}

