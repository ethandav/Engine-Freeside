#include "..\include\SceneManager.h"
#include "..\..\efg.Renderer\include\render\Renderer.h"

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
		}

		void SceneManager::RenderScene(Scene scene, uint64_t frameId)
		{
			efg::FramePacket framePacket = scene.BuildFramePacket(frameId);
			m_renderer->SubmitFrame(std::move(framePacket));
		}
	}
}