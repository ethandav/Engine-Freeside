#pragma once

#include "..\..\..\..\..\Freeside.Core\include\math\Mat4.h"

#include <cstdint>

namespace efg::d3d12
{
	struct GpuSkyboxConstants
	{
		Freeside::Math::Mat4 ViewProjection;
		uint32_t SkyboxTextureDescriptorIndex;
		float Padding[3];
	};
}