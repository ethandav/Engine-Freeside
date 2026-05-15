#pragma once

#include "Scene.h"
#include "..\..\efg.Renderer\include\render\Lights.h"
#include "..\..\efg.Renderer\include\render\Camera.h"

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
			SceneHandle CreateScene(std::wstring sceneName);
			SceneHandle AddSceneToQueue(const Scene& scene);
			SceneRenderObjectHandle AddRenderObjectToRenderQueue(SceneHandle handle, RenderObject& object);
			PointLightHandle AddPointLightToScene(SceneHandle handle, Lights::Point& light);
			RenderObject* GetRenderObjectByHandle(SceneHandle sceneHandle, SceneRenderObjectHandle objectHandle);
			Lights::Point* GetPointLightByHandle(SceneHandle sceneHandle, PointLightHandle pointLightHandle);
			void AddCamera(SceneHandle handle, Camera camera);
			void RenderScene(SceneHandle handle);
		private:
			Scene* GetSceneByHandle(SceneHandle handle);

			Renderer* m_renderer = nullptr;
			std::vector<Scene> m_sceneQueue = {};
		};
	}
}