#pragma once

#include "cube.h"
#include "sphere.h"
#include "pyramid.h"
#include "plane.h"
#include "wall.h"

namespace Freeside::Shapes
{
	enum Types : uint32_t
	{
		CUBE,
		SPHERE,
		PLANE,
		WALL,
		PYRAMID,

		COUNT
	};
}