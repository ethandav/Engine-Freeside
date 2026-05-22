#pragma once

#include "..\..\..\..\..\efg.Core\include\math\Vec4.h"
#include "..\..\..\..\..\efg.Core\include\math\Mat4.h"
#include "..\..\resources\D3D12GpuTexture.h"

namespace efg::d3d12
{
	struct LightViewConstants
	{
		Freeside::Math::Vec4 viewPosition;
		Freeside::Math::Mat4 viewProjection;
	};
}