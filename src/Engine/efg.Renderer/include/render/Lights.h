#pragma once
#include "..\..\..\efg.Core\include\math\Vec3.h"
#include "..\..\..\efg.Core\include\math\Vec4.h"

#include <cstdint>

namespace Freeside
{
	namespace Lights
	{
		static constexpr uint32_t MaxPointLights = 1024;

		class Directional
		{
		public:
			Directional() = default;

			Freeside::Math::Vec3 direction = Freeside::Math::Vec3(0.0f, 01.0f, 0.0f);
			Freeside::Math::Vec3 color = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
		};

		class Point
		{
		public:
			Point() = default;

			Freeside::Math::Vec3 position = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
			float radius = 0.0f;
			Freeside::Math::Vec3 color = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
			float intensity = 1.0f;
		};
	}
}