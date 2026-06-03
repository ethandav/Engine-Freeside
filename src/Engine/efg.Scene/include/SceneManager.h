#pragma once

#include "Scene.h"

#include <vector>

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
			void RenderScene(Scene scene, uint64_t frameId);
		private:
			Renderer* m_renderer = nullptr;
			std::vector<Scene> m_sceneQueue = {};

		};
	}
}