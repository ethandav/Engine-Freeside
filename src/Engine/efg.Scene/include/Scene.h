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

		class Scene
		{
		public:
			Scene(std::wstring name);
			void AddRenderObjectToRenderQueue(RenderObject& object);
			void AddCamera(Camera* camera);
			void Render(Renderer* renderer);

			std::wstring name;
		private:
			uint32_t handle = 0;
			Camera* m_camera = nullptr;
			efg::Lights::Directional dirLight;
			std::vector<RenderObject*> m_renderObjectQueue = {};
		};
	}
}