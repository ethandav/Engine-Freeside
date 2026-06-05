#pragma once

#include "..\..\..\..\..\Freeside.Core\include\math\Vec4.h"
#include "..\..\..\..\..\Freeside.Core\include\math\Mat4.h"

namespace efg::d3d12
{
	struct LightViewConstants
	{
		Freeside::Math::Vec4 viewPosition;
		Freeside::Math::Mat4 viewProjection;
	};

	struct PointShadowConstants
	{
		Freeside::Math::Vec4 positionAndFarPlane;
	};
}