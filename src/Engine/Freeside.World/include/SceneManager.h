#pragma once

#include "Scene.h"
#include "SceneSerializer.h"

#include <vector>
#include <filesystem>

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
			void RenderScene(Scene scene, uint64_t frameId);
		private:
			Renderer* m_renderer = nullptr;
			SceneSerializer m_sceneSerializer = {};
			std::vector<Scene> m_sceneQueue = {};

		};
	}
}