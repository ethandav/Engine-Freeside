#include "..\include\Win32Application.h"
#include "..\..\Engine\Freeside.Core\freeside.h"
#include "..\..\Engine\Freeside.Core\include\controllers\FirstPersonCameraController.h"

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
	Freeside::Assets::AssetManager assets;

	Freeside::RendererDesc rendererDesc = {
		nullptr,
		1920,
		1080
	};

	Freeside::MeshDesc cubeMeshData = Freeside::Shapes::Cube().mesh;
	Freeside::MeshDesc planeMeshData = Freeside::Shapes::Plane().mesh;
	Freeside::MeshDesc wallMeshData = Freeside::Shapes::Wall().mesh;

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
	window.SetCursorLocked(true);

	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);
	float aspectRatio = renderer.GetRendererAspectRatio();
	sceneManager.Initialize(&renderer);
	assets.Initialize(&renderer);

	Freeside::MeshHandle cubeMeshHandle = assets.CreateMesh(cubeMeshData);
	Freeside::MeshHandle planeMeshHandle = assets.CreateMesh(planeMeshData);
	Freeside::MeshHandle wallMeshHandle = assets.CreateMesh(wallMeshData);

	Freeside::TextureHandle hCrateColorTexture = assets.CreateTextureFromImagePath(L"assets/textures/crate.png");
	Freeside::TextureHandle hCrateNormalTexture = assets.CreateTextureFromImagePath(L"assets/textures/crate_normals.png");
	Freeside::TextureHandle hCrateHeightTexture = assets.CreateTextureFromImagePath(L"assets/textures/crate_height.png");
	Freeside::TextureHandle hWallColorTexture = assets.CreateTextureFromImagePath(L"assets/textures/brick.png");
	Freeside::TextureHandle hWallNormalTexture = assets.CreateTextureFromImagePath(L"assets/textures/brick_normals.png");
	Freeside::TextureHandle hWallHeightTexture = assets.CreateTextureFromImagePath(L"assets/textures/brick_height.png");
	Freeside::TextureHandle hFloorColorTexture = assets.CreateTextureFromImagePath(L"assets/textures/floor.png");
	Freeside::TextureHandle hFloorNormalTexture = assets.CreateTextureFromImagePath(L"assets/textures/floor_normals.png");
	Freeside::TextureHandle hFloorHeightTexture = assets.CreateTextureFromImagePath(L"assets/textures/floor_height.png");

	Freeside::MaterialDesc crateMaterial;
	crateMaterial.baseColorFactor = Freeside::Math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	crateMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	crateMaterial.metallicFactor = 0.0f;
	crateMaterial.roughnessFactor = 1.0f;
	crateMaterial.baseColorTexture = hCrateColorTexture;
	crateMaterial.normalTexture = hCrateNormalTexture;
	crateMaterial.heightTexture = hCrateHeightTexture;
	Freeside::MaterialHandle crateMaterialHandle = assets.CreateMaterial(crateMaterial);

	Freeside::MaterialDesc wallMaterial;
	wallMaterial.baseColorFactor = Freeside::Math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	wallMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	wallMaterial.uvScale = Freeside::Math::Vec2(2.0f, 2.0f);
	wallMaterial.metallicFactor = 0.0f;
	wallMaterial.roughnessFactor = 1.0f;
	wallMaterial.baseColorTexture = hWallColorTexture;
	wallMaterial.normalTexture = hWallNormalTexture;
	wallMaterial.heightTexture = hWallHeightTexture;
	Freeside::MaterialHandle wallMaterialHandle = assets.CreateMaterial(wallMaterial);

	Freeside::MaterialDesc floorMaterial;
	floorMaterial.baseColorFactor = Freeside::Math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	floorMaterial.specular = Freeside::Math::Vec2(1.0f, 64.0f);
	floorMaterial.uvScale = Freeside::Math::Vec2(2.0f, 2.0f);
	floorMaterial.metallicFactor = 0.0f;
	floorMaterial.roughnessFactor = 1.0f;
	floorMaterial.baseColorTexture = hFloorColorTexture;
	floorMaterial.normalTexture = hFloorNormalTexture;
	floorMaterial.heightTexture = hFloorHeightTexture;
	Freeside::MaterialHandle floorMaterialHandle = assets.CreateMaterial(floorMaterial);

	Freeside::Scene::Scene testScene(L"Test Scene");

	Freeside::Assets::ImportedModel model = assets.ImportModel("assets\\models\\DamagedHelmet.glb");
	for (const Freeside::Assets::ImportedMesh& mesh : model.meshes)
	{
		for (const Freeside::Assets::ImportedPrimitive& prim : mesh.primitives)
		{
			Freeside::MeshHandle mesh = assets.CreateMesh(prim.meshData);
			Freeside::Entity eMesh = testScene.CreateEntity();
			Freeside::MeshRendererComponent& cMeshRenderer = testScene.AddMeshRenderer(eMesh);
			Freeside::TransformComponent& cMeshTransform = testScene.AddTransform(eMesh);
			Freeside::MaterialDesc importMat = assets.ConvertGLTFMaterial(model.materials[prim.materialIndex], model.textures);
			Freeside::MaterialHandle importMatHandle = assets.CreateMaterial(importMat);

			cMeshRenderer.material = importMatHandle;
			cMeshRenderer.mesh = mesh;
			cMeshTransform.position = Freeside::Math::Vec3(0.0f, 1.0f, -1.0f);
			cMeshTransform.rotation = Freeside::Math::Vec3(-Freeside::Math::PI * 0.5f, Freeside::Math::PI * 1.0f, 0.0f);
			cMeshTransform.scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
		}
	}

	Freeside::Entity eCamera = testScene.CreateEntity();
	Freeside::CameraComponent& cCamera = testScene.AddCamera(eCamera);
	cCamera.camera.SetPosition(Freeside::Math::Vec3(0.0f, 1.0f, -5.0f));
	cCamera.camera.SetTarget(Freeside::Math::Vec3(0.0f, 1.0f, 0.0f));
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
	cPointLight.intensity = 0.0f;
	cPointLight.radius = 10.0f;
	cPointLightTransform.position = Freeside::Math::Vec3(0.0f, 0.5f, -1.0f);

	Freeside::Entity ePointLight2 = testScene.CreateEntity();
	Freeside::PointLightComponent& cPointLight2 = testScene.AddPointLight(ePointLight2);
	Freeside::TransformComponent& cPointLightTransform2 = testScene.AddTransform(ePointLight2);
	cPointLight2.intensity = 3.0f;
	cPointLight2.radius = 10.0f;
	cPointLightTransform2.position = Freeside::Math::Vec3(0.0f, 0.5f, -3.0f);

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
		totalTime += deltaTime;

		Freeside::InputState input = window.PollInput();

		cameraController.Update(&cCamera.camera, input, deltaTime);

		sceneManager.RenderScene(testScene, frameId);
	}
}

