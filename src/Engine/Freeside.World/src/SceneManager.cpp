#include "..\include\SceneManager.h"
#include "..\..\efg\include\render\Renderer.h"
#include "..\..\Freeside.Core\include\shapes\shapes.h"
#include "..\..\Freeside.Core\include\math\MatrixTransform.h"

#include <stdexcept>

namespace Freeside
{
	namespace Scene
	{
		SceneManager::SceneManager()
		{
		}

		SceneManager::~SceneManager()
		{
		}

		void SceneManager::Initialize(Renderer* renderer)
		{
			m_renderer = renderer;
			m_assetManager.Initialize(renderer);
		}

		void SceneManager::LoadScene(const std::filesystem::path& path)
		{
			m_sceneQueue.push_back(SceneLoadRequest{ path });
		}

		void SceneManager::RenderScene(uint64_t frameId)
		{
			if (!m_activeScene || !m_renderer)
				return;

			efg::FramePacket framePacket = m_activeScene->BuildFramePacket(frameId);
			m_renderer->SubmitFrame(std::move(framePacket));
		}

		void SceneManager::Update(InputState input, const float deltaTime)
		{
			if (!m_activeScene)
				TryLoadNextScene();

			if (!m_activeScene)
				return;

			m_activeScene->Update(input, deltaTime);
		}

		void SceneManager::TryLoadNextScene()
		{
			if (m_activeScene)
				return;

			if (m_sceneQueue.empty())
				return;

			SceneLoadRequest request = m_sceneQueue.front();
			m_sceneQueue.pop_front();

			m_activeScene = std::make_unique<Scene>(L"Loaded Scene");

			m_sceneSerializer.Load(*m_activeScene, request.path);
		}

		void SceneManager::CreateDefaultScene()
		{
			m_activeScene = std::make_unique<Scene>(L"Test Scene");

			MeshDesc cubeMeshData = Shapes::Cube().mesh;
			MeshDesc planeMeshData = Shapes::Plane().mesh;
			MeshDesc wallMeshData = Shapes::Wall().mesh;

			MeshHandle cubeMeshHandle = m_assetManager.CreateMesh(cubeMeshData);
			MeshHandle planeMeshHandle = m_assetManager.CreateMesh(planeMeshData);
			MeshHandle wallMeshHandle = m_assetManager.CreateMesh(wallMeshData);

			TextureHandle hCrateColorTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/crate.png");
			TextureHandle hCrateNormalTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/crate_normals.png");
			TextureHandle hCrateHeightTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/crate_height.png");
			TextureHandle hWallColorTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/brick.png");
			TextureHandle hWallNormalTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/brick_normals.png");
			TextureHandle hWallHeightTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/brick_height.png");
			TextureHandle hFloorColorTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/floor.png");
			TextureHandle hFloorNormalTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/floor_normals.png");
			TextureHandle hFloorHeightTexture = m_assetManager.CreateTextureFromImagePath(L"assets/textures/floor_height.png");

			std::array<std::wstring, 6> skyboxImages = {
				L"assets/textures/skybox/right.jpg",
				L"assets/textures/skybox/left.jpg",
				L"assets/textures/skybox/top.jpg",
				L"assets/textures/skybox/bottom.jpg",
				L"assets/textures/skybox/front.jpg",
				L"assets/textures/skybox/back.jpg",
			};

			TextureHandle hSkyBoxTexture = m_assetManager.CreateSkyboxFromImagePaths(skyboxImages);

			MaterialDesc crateMaterial;
			crateMaterial.baseColorFactor = Math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			crateMaterial.specular = Math::Vec2(1.0f, 64.0f);
			crateMaterial.metallicFactor = 0.0f;
			crateMaterial.roughnessFactor = 1.0f;
			crateMaterial.baseColorTexture = hCrateColorTexture;
			crateMaterial.normalTexture = hCrateNormalTexture;
			crateMaterial.heightTexture = hCrateHeightTexture;
			MaterialHandle crateMaterialHandle = m_assetManager.CreateMaterial(crateMaterial);

			MaterialDesc wallMaterial;
			wallMaterial.baseColorFactor = Math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			wallMaterial.specular = Math::Vec2(1.0f, 64.0f);
			wallMaterial.uvScale = Math::Vec2(2.0f, 2.0f);
			wallMaterial.metallicFactor = 0.0f;
			wallMaterial.roughnessFactor = 1.0f;
			wallMaterial.baseColorTexture = hWallColorTexture;
			wallMaterial.normalTexture = hWallNormalTexture;
			wallMaterial.heightTexture = hWallHeightTexture;
			MaterialHandle wallMaterialHandle = m_assetManager.CreateMaterial(wallMaterial);

			MaterialDesc floorMaterial;
			floorMaterial.baseColorFactor = Math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			floorMaterial.specular = Math::Vec2(1.0f, 64.0f);
			floorMaterial.uvScale = Math::Vec2(2.0f, 2.0f);
			floorMaterial.metallicFactor = 0.0f;
			floorMaterial.roughnessFactor = 1.0f;
			floorMaterial.baseColorTexture = hFloorColorTexture;
			floorMaterial.normalTexture = hFloorNormalTexture;
			floorMaterial.heightTexture = hFloorHeightTexture;
			MaterialHandle floorMaterialHandle = m_assetManager.CreateMaterial(floorMaterial);

			Assets::ImportedModel model = m_assetManager.ImportModel("assets\\models\\DamagedHelmet.glb");
			for (int rootNodeIndex : model.rootNodes)
			{
				Entity root = m_activeScene->CreateEntityFromImportedNode(
					&m_assetManager,
					model,
					rootNodeIndex,
					Entity(-1)
				);

				TransformComponent* rootTransform = m_activeScene->GetTransform(root);

				// Optional model placement in sandbox:
				rootTransform->position += Math::Vec3(0.0f, 1.0f, 0.0f);
			}

			Entity eCamera = m_activeScene->CreateEntity();
			CameraComponent& cCamera = m_activeScene->AddCamera(eCamera);
			TransformComponent& cCameraTransform = m_activeScene->AddTransform(eCamera);
			cCameraTransform.position = {0.0f, 1.0f, 5.0f};
			cCameraTransform.rotation = Math::RotationLookingAt(
				cCameraTransform.position,
				Math::Vec3(0.0f, 0.5f, 0.0f)
			);
			cCamera.isMainCamera = true;
			FirstPersonCameraControllerComponent& cCameraController = m_activeScene->AddFirstPersonCameraControllerComponent(eCamera);

			/*
			Entity eCrate = testScene.CreateEntity();
			MeshRendererComponent& cCrateRenderer = testScene.AddMeshRenderer(eCrate);
			TransformComponent& cCrateTransform = testScene.AddTransform(eCrate);
			cCrateRenderer.material = crateMaterialHandle;
			cCrateRenderer.mesh = cubeMeshHandle;
			cCrateTransform.position = Math::Vec3(-1.0f, 0.0f, 0.0f);
			cCrateTransform.rotation = Math::Vec3(0.0f, Math::PI * 0.1f, 0.0f);
			cCrateTransform.scale = Math::Vec3(1.0f, 1.0f, 1.0f);

			Entity eCrate2 = testScene.CreateEntity();
			MeshRendererComponent& cCrate2Renderer = testScene.AddMeshRenderer(eCrate2);
			TransformComponent& cCrate2Transform = testScene.AddTransform(eCrate2);
			cCrate2Renderer.material = crateMaterialHandle;
			cCrate2Renderer.mesh = cubeMeshHandle;
			cCrate2Transform.position = Math::Vec3(1.0f, 0.0f, 0.0f);
			cCrate2Transform.rotation = Math::Vec3(0.0f, Math::PI * 0.2f, 0.0f);
			cCrate2Transform.scale = Math::Vec3(1.0f, 1.0f, 1.0f);

			Entity eCrate3 = testScene.CreateEntity();
			MeshRendererComponent& cCrate3Renderer = testScene.AddMeshRenderer(eCrate3);
			TransformComponent& cCrate3Transform = testScene.AddTransform(eCrate3);
			cCrate3Renderer.material = crateMaterialHandle;
			cCrate3Renderer.mesh = cubeMeshHandle;
			cCrate3Transform.position = Math::Vec3(1.0f, 1.0f, 0.0f);
			cCrate3Transform.rotation = Math::Vec3(0.0f, 0.0f, 0.0f);
			cCrate3Transform.scale = Math::Vec3(1.0f, 1.0f, 1.0f);
			*/

			Entity eWall1 = m_activeScene->CreateEntity();
			MeshRendererComponent& cWall1Renderer = m_activeScene->AddMeshRenderer(eWall1);
			TransformComponent& cWall1Transform = m_activeScene->AddTransform(eWall1);
			cWall1Renderer.material = wallMaterialHandle;
			cWall1Renderer.mesh = wallMeshHandle;
			cWall1Transform.position = Math::Vec3(0.0f, 2.0f, -2.5f);
			cWall1Transform.rotation = Math::Quat(0.0f, 0.0f, 0.0f, 0.0f);
			cWall1Transform.scale = Math::Vec3(5.0f, 5.0f, 5.0f);

			Entity eWall2 = m_activeScene->CreateEntity();
			MeshRendererComponent& cWall2Renderer = m_activeScene->AddMeshRenderer(eWall2);
			TransformComponent& cWall2Transform = m_activeScene->AddTransform(eWall2);
			cWall2Renderer.material = wallMaterialHandle;
			cWall2Renderer.mesh = wallMeshHandle;
			cWall2Transform.position = Math::Vec3(-2.5f, 2.0f, 0.0f);;
			cWall2Transform.rotation = Math::FromAxisAngle(Math::Vec3(0.0f, 1.0f, 0.0f), Math::PIDIV2);
			cWall2Transform.scale = Math::Vec3(5.0f, 5.0f, 5.0f);

			Entity eWall3 = m_activeScene->CreateEntity();
			MeshRendererComponent& cWall3Renderer = m_activeScene->AddMeshRenderer(eWall3);
			TransformComponent& cWall3Transform = m_activeScene->AddTransform(eWall3);
			cWall3Renderer.material = wallMaterialHandle;
			cWall3Renderer.mesh = wallMeshHandle;
			cWall3Transform.position = Math::Vec3(2.5f, 2.0f, 0.0f);
			cWall3Transform.rotation = Math::FromAxisAngle(Math::Vec3(0.0f, 1.0f, 0.0f), -Math::PIDIV2);
			cWall3Transform.scale = Math::Vec3(5.0f, 5.0f, 5.0f);

			Entity eFloor = m_activeScene->CreateEntity();
			MeshRendererComponent& cFloorRenderer = m_activeScene->AddMeshRenderer(eFloor);
			TransformComponent& cFloorTransform = m_activeScene->AddTransform(eFloor);
			cFloorRenderer.material = floorMaterialHandle;
			cFloorRenderer.mesh = planeMeshHandle;
			cFloorTransform.position = Math::Vec3(0.0f, -0.5f, 0.0f);
			cFloorTransform.rotation = Math::Quat(0.0f, 0.0f, 0.0f, 0.0f);
			cFloorTransform.scale = Math::Vec3(5.0f, 5.0f, 5.0f);

			Entity ePointLight2 = m_activeScene->CreateEntity();
			PointLightComponent& cPointLight2 = m_activeScene->AddPointLight(ePointLight2);
			TransformComponent& cPointLightTransform2 = m_activeScene->AddTransform(ePointLight2);
			cPointLight2.intensity = 5.0f;
			cPointLight2.radius = 10.0f;
			cPointLightTransform2.position = Math::Vec3(-2.0f, 3.0f, 2.0f);

			Entity eDirLight = m_activeScene->CreateEntity();
			DirectionalLightComponent& cDirLight = m_activeScene->AddDirectionalLight(eDirLight);
			cDirLight.intensity = 5.0f;
			cDirLight.color = Math::Vec3(1.0f, 1.0f, 1.0f);
			cDirLight.direction = Math::Vec3(0.2f, -1.0f, -0.3f);
		}
	}
}