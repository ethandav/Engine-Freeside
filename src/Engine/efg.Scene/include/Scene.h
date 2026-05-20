#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "..\..\efg.Renderer\include\render\types\RenderObject.h"
#include "..\..\efg.Renderer\include\render\Lights.h"
#include "..\..\efg.Renderer\include\render\Camera.h"

namespace Freeside
{
	class Renderer;

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
			PointLightHandle AddPointLightToScene(Lights::Point light);
			DirectionalLightHandle AddDirectionalLightToScene(Lights::Directional light);
			RenderObject* GetRenderObjectByHandle(SceneRenderObjectHandle handle);
			Lights::Directional* GetDirectionalLightByHandle(DirectionalLightHandle handle);
			Lights::Point* GetPointLightByHandle(PointLightHandle handle);
			void CreateScenefromDefault(float aspectRatio);
			void AddCamera(Camera camera);
			void Render(Renderer* renderer);

			std::wstring name;
		private:

			uint32_t handle = 0;
			uint32_t pointLightCount = 0;
			uint32_t directionalLightCount = 0;
			uint32_t objectCount = 0;
			Camera m_camera = {};
			std::vector<Lights::Point> m_pointLights = {};
			std::vector<Lights::Directional> m_directionalLights = {};
			std::vector<RenderObject> m_renderObjectQueue = {};
		};
	}
}