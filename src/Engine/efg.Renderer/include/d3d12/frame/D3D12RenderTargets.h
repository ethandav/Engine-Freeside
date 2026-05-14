#pragma once
#include "..\resources\D3D12GpuTexture.h"

namespace efg::d3d12
{
	struct RenderTargets
	{
		GpuTexture2D sceneColor;
		GpuDepthBuffer sceneDepth;
	};
}