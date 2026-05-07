#pragma once
#include "..\..\efg.Core\include\math\Vec3.h"
#include "..\..\efg.Core\include\math\Vec4.h"

namespace efg
{
	namespace Lights
	{
		struct DirectionalLightConstants
		{
			Vec4 directionAndIntensity; // xyz = direction, w = intensity
			Vec4 colorAndPadding;       // rgb = color, w unused
		};

		class Directional
		{
		public:
			Directional() = default;
			DirectionalLightConstants BuildDirectionalLightConstants() const;

			Vec3 direction = Vec3(0.0f, 01.0f, 0.0f);
			Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
		};
	}
}