#pragma once

#include "..\..\..\..\..\efg.Core\include\math\Vec4.h"
#include "..\..\..\..\..\efg.Core\include\math\Mat4.h"
#include "..\..\resources\D3D12GpuTexture.h"

namespace efg::d3d12
{
	struct LightViewConstants
	{
		Freeside::Math::Mat4 viewProjection;
	};

	struct ShaderMapRenderPassInputs
	{

	};

	struct ShaderMapRenderPassOutputs
	{

	};

	struct ShadowMapFrameData
	{
		const GpuDepthBuffer* shadowMap = nullptr;
		Freeside::Math::Mat4 lightViewProjection = {};
		uint32_t width = 0;
		uint32_t height = 0;
	};
}