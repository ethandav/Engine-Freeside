#pragma once
#include <cstdint>
#include <string>

#include "..\..\efg.Renderer\include\Renderer.h"

namespace efg
{
	namespace Scene
	{
		struct SceneHandle
		{
			uint32_t index = UINT32_MAX;

			bool IsValid() const
			{
				return index != UINT32_MAX;
			}
		};

		struct SceneRenderObjectHandle
		{
			uint32_t index = UINT32_MAX;

			bool IsValid() const
			{
				return index != UINT32_MAX;
			}
		};

		class Scene
		{
		public:
			Scene(std::wstring name);
			SceneRenderObjectHandle AddRenderObjectToRenderQueue(RenderObject object);
			void AddPointLightToScene(efg::Lights::Point light);
			RenderObject* GetRenderObjectByHandle(SceneRenderObjectHandle handle);
			void CreateScenefromDefault(float aspectRatio);
			void AddCamera(Camera camera);
			void Render(Renderer* renderer);

			std::wstring name;
		private:

			uint32_t handle = 0;
			uint32_t pointLightCount = 0;
			uint32_t objectCount = 0;
			Camera m_camera = {};
			Lights::Directional m_dirLight = {};
			std::vector<Lights::Point> m_pointLights = {};
			std::vector<RenderObject> m_renderObjectQueue = {};
		};
	}
}