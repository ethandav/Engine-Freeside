#include "..\include\SceneManager.h"
#include "..\..\efg\include\render\Renderer.h"
#include "..\..\Freeside.Core\include\shapes\shapes.h"
#include "..\..\Freeside.Core\include\math\MatrixTransform.h"
#include "..\include\SceneImporter.h"

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
#if defined(EFG_RENDER_IMMEDIATE)
			m_renderer->RenderImmediate(framePacket);
#else
			m_renderer->SubmitFrame(std::move(framePacket));
#endif
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

			m_sceneSerializer.Load(*m_activeScene, request.path, m_assetManager);
		}

		void SceneManager::CreateDefaultScene()
		{
			LoadScene(L"scenes\\testScene\\test_scene.json");
		}
	}
}