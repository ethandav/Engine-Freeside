#pragma once
#include "D3D12FrameResouce.h"
#include "..\..\render\RenderQueue.h"

#include <d3d12.h>
#include <cstdint>

namespace efg::d3d12
{
	struct FrameContext
	{
		uint64_t frameIndex = 0;
		FrameResource* frame = nullptr;
		ID3D12GraphicsCommandList* commandList = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle = {};
		ID3D12Resource* backBuffer = nullptr;
		RenderQueue* renderQueue = nullptr;
	};
}