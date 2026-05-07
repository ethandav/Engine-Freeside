#pragma once
#include "..\..\efg.Core\include\math\Vec4.h"
#include "..\..\efg.Core\include\math\Mat4.h"

namespace efg
{
	struct ObjectConstants
	{
		Mat4 world;
	};

	struct MaterialConstants
	{
		Vec4 baseColor;
		Vec4 specular;
	};

	struct DirectionalLightConstants
	{
		Vec4 directionAndIntensity; // xyz = direction, w = intensity
		Vec4 colorAndPadding;       // rgb = color, w unused
	};
}