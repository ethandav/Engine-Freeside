#include "..\include\Win32Application.h"
#include "..\..\Engine\Freeside.Core\freeside.h"
#include "..\..\Engine\Freeside.Core\include\controllers\FirstPersonCameraController.h"

#include <random>

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

	window.Create(hInstance, rendererDesc.width, rendererDesc.height, L"Ethan's Framework (for) Graphics");
	window.Show(nCmdShow);
#ifndef FREESIDE_EDITOR
	window.SetCursorLocked(true);
#endif

#if defined(EFG_ENABLE_IMGUI)
	window.SetNativeMessageHandler(
		[&renderer](
			void* hwnd,
			uint32_t message,
			uintptr_t wParam,
			intptr_t lParam,
			intptr_t& result)
		{
			return renderer.HandleNativeWindowMessage(
				hwnd,
				message,
				wParam,
				lParam,
				result
			);
		}
	);
#endif

	rendererDesc.nativeWindowHandle = window.GetHwnd();
	renderer.Initialize(rendererDesc);
	float aspectRatio = renderer.GetRendererAspectRatio();
	sceneManager.Initialize(&renderer);
	assets.Initialize(&renderer);

	sceneManager.LoadScene("scenes\\testScene\\test_scene.json");

	sceneManager.CreateDefaultScene();

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

#if defined(EFG_RENDER_IMMEDIATE)
		renderer.StartFrame();
#endif
		sceneManager.Update(input, deltaTime);
		sceneManager.RenderScene(frameId);

#if defined(EFG_RENDER_IMMEDIATE)
		renderer.EndFrame();
#endif
	}
}

