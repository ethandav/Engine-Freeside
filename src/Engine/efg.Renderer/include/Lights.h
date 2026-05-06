#pragma once
#include "..\..\efg.Core\include\math\Vec3.h"

namespace efg
{
	namespace Lights
	{
		struct Directional
		{
			Vec3 direction = Vec3(0.0f, 01.0f, 0.0f);
			Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
		};
	}
}