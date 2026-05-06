#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Platform\include\Window.h"
#include "..\..\Engine\efg.Platform\include\Timer.h"
#include "..\..\Engine\efg.Renderer\include\Renderer.h"
#include "..\..\Engine\efg.Core\include\math\math.h"
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

    uint32_t vertexCount = 24;
    uint32_t indexCount = 36;
    cubeMeshData.vertices = std::vector<efg::Vertex>(vertexCount);
    cubeMeshData.indices = std::vector<uint32_t>(indexCount);

    cubeMeshData.indices = {
         2,  1,  0, // Top
         3,  1,  2,
         6,  5,  4, // Back
         7,  5,  6,
        10,  9,  8, // Right
        11,  9, 10,
        14, 13, 12, // Front
        15, 13, 14,
        18, 17, 16, // Left
        19, 17, 18,
        22, 21, 20, // Bottom
        23, 21, 22
    };

    cubeMeshData.vertices[0].position = efg::Vec3(-0.5f, 0.5f, -0.5f); //Top
    cubeMeshData.vertices[1].position = efg::Vec3(0.5f, 0.5f, -0.5f);
    cubeMeshData.vertices[2].position = efg::Vec3(-0.5f, 0.5f, 0.5f);
    cubeMeshData.vertices[3].position = efg::Vec3(0.5f, 0.5f, 0.5f);
    cubeMeshData.vertices[4].position = efg::Vec3(-0.5f, 0.5f, 0.5f); //Front
    cubeMeshData.vertices[5].position = efg::Vec3(0.5f, 0.5f, 0.5f);
    cubeMeshData.vertices[6].position = efg::Vec3(-0.5f, -0.5f, 0.5f);
    cubeMeshData.vertices[7].position = efg::Vec3(0.5f, -0.5f, 0.5f);
    cubeMeshData.vertices[8].position = efg::Vec3(0.5f, 0.5f, 0.5f); //Right
    cubeMeshData.vertices[9].position = efg::Vec3(0.5f, 0.5f, -0.5f);
    cubeMeshData.vertices[10].position = efg::Vec3(0.5f, -0.5f, 0.5f);
    cubeMeshData.vertices[11].position = efg::Vec3(0.5f, -0.5f, -0.5f);
    cubeMeshData.vertices[12].position = efg::Vec3(0.5f, 0.5f, -0.5f); //Back
    cubeMeshData.vertices[13].position = efg::Vec3(-0.5f, 0.5f, -0.5f);
    cubeMeshData.vertices[14].position = efg::Vec3(0.5f, -0.5f, -0.5f);
    cubeMeshData.vertices[15].position = efg::Vec3(-0.5f, -0.5f, -0.5f);
    cubeMeshData.vertices[16].position = efg::Vec3(-0.5f, 0.5f, -0.5f); //Left
    cubeMeshData.vertices[17].position = efg::Vec3(-0.5f, 0.5f, 0.5f);
    cubeMeshData.vertices[18].position = efg::Vec3(-0.5f, -0.5f, -0.5f);
    cubeMeshData.vertices[19].position = efg::Vec3(-0.5f, -0.5f, 0.5f);
    cubeMeshData.vertices[20].position = efg::Vec3(-0.5f, -0.5f, 0.5f); //Bottom
    cubeMeshData.vertices[21].position = efg::Vec3(0.5f, -0.5f, 0.5f);
    cubeMeshData.vertices[22].position = efg::Vec3(-0.5f, -0.5f, -0.5f);
    cubeMeshData.vertices[23].position = efg::Vec3(0.5f, -0.5f, -0.5f);

    /*
    cube.vertices[0].normal = glm::vec3(0.0f, 1.0f, 0.0f); //Top
    cube.vertices[1].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    cube.vertices[2].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    cube.vertices[3].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    cube.vertices[4].normal = glm::vec3(0.0f, 0.0f, 1.0f); //Front
    cube.vertices[5].normal = glm::vec3(0.0f, 0.0f, 1.0f);
    cube.vertices[6].normal = glm::vec3(0.0f, 0.0f, 1.0f);
    cube.vertices[7].normal = glm::vec3(0.0f, 0.0f, 1.0f);
    cube.vertices[8].normal = glm::vec3(1.0f, 0.0f, 0.0f); //Right
    cube.vertices[9].normal = glm::vec3(1.0f, 0.0f, 0.0f);
    cube.vertices[10].normal = glm::vec3(1.0f, 0.0f, 0.0f);
    cube.vertices[11].normal = glm::vec3(1.0f, 0.0f, 0.0f);
    cube.vertices[12].normal = glm::vec3(0.0f, 0.0f, -1.0f); //Back
    cube.vertices[13].normal = glm::vec3(0.0f, 0.0f, -1.0f);
    cube.vertices[14].normal = glm::vec3(0.0f, 0.0f, -1.0f);
    cube.vertices[15].normal = glm::vec3(0.0f, 0.0f, -1.0f);
    cube.vertices[16].normal = glm::vec3(-1.0f, 0.0f, 0.0f); //Left
    cube.vertices[17].normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    cube.vertices[18].normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    cube.vertices[19].normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    cube.vertices[20].normal = glm::vec3(0.0f, -1.0f, 0.0f); //Bottom
    cube.vertices[21].normal = glm::vec3(0.0f, -1.0f, 0.0f);
    cube.vertices[22].normal = glm::vec3(0.0f, -1.0f, 0.0f);
    cube.vertices[23].normal = glm::vec3(0.0f, -1.0f, 0.0f);

    // Top face
    cube.vertices[0].uv = glm::vec2(0.0f, 0.0f);
    cube.vertices[1].uv = glm::vec2(1.0f, 0.0f);
    cube.vertices[2].uv = glm::vec2(0.0f, 1.0f);
    cube.vertices[3].uv = glm::vec2(1.0f, 1.0f);
    // Front face
    cube.vertices[4].uv = glm::vec2(0.0f, 0.0f);
    cube.vertices[5].uv = glm::vec2(1.0f, 0.0f);
    cube.vertices[6].uv = glm::vec2(0.0f, 1.0f);
    cube.vertices[7].uv = glm::vec2(1.0f, 1.0f);

    // Right face
    cube.vertices[8].uv = glm::vec2(0.0f, 0.0f);
    cube.vertices[9].uv = glm::vec2(1.0f, 0.0f);
    cube.vertices[10].uv = glm::vec2(0.0f, 1.0f);
    cube.vertices[11].uv = glm::vec2(1.0f, 1.0f);
    // Back face
    cube.vertices[12].uv = glm::vec2(0.0f, 0.0f);
    cube.vertices[13].uv = glm::vec2(1.0f, 0.0f);
    cube.vertices[14].uv = glm::vec2(0.0f, 1.0f);
    cube.vertices[15].uv = glm::vec2(1.0f, 1.0f);
    // Left face
    cube.vertices[16].uv = glm::vec2(0.0f, 0.0f);
    cube.vertices[17].uv = glm::vec2(1.0f, 0.0f);
    cube.vertices[18].uv = glm::vec2(0.0f, 1.0f);
    cube.vertices[19].uv = glm::vec2(1.0f, 1.0f);
    // Bottom face
    cube.vertices[20].uv = glm::vec2(0.0f, 0.0f);
    cube.vertices[21].uv = glm::vec2(1.0f, 0.0f);
    cube.vertices[22].uv = glm::vec2(0.0f, 1.0f);
    cube.vertices[23].uv = glm::vec2(1.0f, 1.0f);
    */

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

