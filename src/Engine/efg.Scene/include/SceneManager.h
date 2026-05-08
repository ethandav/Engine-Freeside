#pragma once
#include "..\include\Scene.h"
#include "..\..\efg.Renderer\include\Renderer.h"

#include <vector>

namespace efg
{
	namespace Scene
	{
		class SceneManager
		{
		public:
			SceneManager();
			~SceneManager();

			void Initialize(Renderer* renderer);
			SceneHandle CreateScene(std::wstring sceneName);
			SceneHandle AddSceneToQueue(const Scene& scene);
			SceneRenderObjectHandle AddRenderObjectToRenderQueue(SceneHandle handle, RenderObject& object);
			void AddPointLightToScene(SceneHandle handle, efg::Lights::Point& light);
			RenderObject* GetRenderObjectByHandle(SceneHandle sceneHandle, SceneRenderObjectHandle objectHandle);
			void AddCamera(SceneHandle handle, Camera camera);
			void RenderScene(SceneHandle handle);
		private:
			Scene* GetSceneByHandle(SceneHandle handle);

			Renderer* m_renderer = nullptr;
			std::vector<Scene> m_sceneQueue = {};
		};
	}
}