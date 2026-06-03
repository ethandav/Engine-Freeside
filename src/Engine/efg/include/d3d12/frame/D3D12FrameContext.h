#pragma once
#include "D3D12FrameResouce.h"
#include "..\..\render\RenderQueue.h"
#include "..\..\d3d12\commands\D3D12CommandContext.h"

#include <d3d12.h>
#include <cstdint>

namespace efg::d3d12
{
	struct FrameContext
	{
		uint64_t frameIndex = 0;
		FrameResource* frameResource = nullptr;
		D3D12DirectCommandContext* commandContext = nullptr;
		D3D12DeviceContext* graphicsContext = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle = {};
		ID3D12Resource* backBuffer = nullptr;
		RenderQueue* renderQueue = nullptr;
	};
}