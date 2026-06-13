#pragma once

#include "Scene.h"
#include "SceneSerializer.h"
#include "..\..\Freeside.Platform\include\InputState.h"
#include "..\..\Freeside.Assets\include\AssetManager.h"

#include <vector>
#include <filesystem>
#include <deque>
#include <memory>

namespace Freeside
{
	class Renderer;

	namespace Scene
	{
		class SceneManager
		{
		public:
			SceneManager();
			~SceneManager();

			void Initialize(Renderer* renderer);
			void LoadScene(const std::filesystem::path& path);
			void SaveActiveScene(const std::filesystem::path& path);
			void RenderScene(uint64_t frameId);
			void Update(Freeside::InputState input, const float deltaTime);
			void TryLoadNextScene();
			void CreateDefaultScene();
			void CreateBlankScene();
			Scene* GetActiveScene();
#if defined(FREESIDE_EDITOR)
			Assets::AssetManager* Assets();
#endif
		private:
			Renderer* m_renderer = nullptr;
			Assets::AssetManager m_assetManager = {};
			SceneSerializer m_sceneSerializer = {};
			std::deque<SceneLoadRequest> m_sceneQueue;
			std::unique_ptr<Scene> m_activeScene;
		};
	}
}