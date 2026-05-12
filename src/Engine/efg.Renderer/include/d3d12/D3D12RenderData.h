#pragma once

#include "D3D12BufferData.h"
#include "d3d12.h"

#include <wrl.h>
#include <stdint.h>

namespace efg::d3d12
{
	static constexpr UINT NumFramesInFlight = 2;

	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		UINT64 fenceValue = 0;

		GpuUploadBufferArena uploadBufferArena = {};
		GpuConstantBufferArena constantBufferArena = {};
		GpuDepthBuffer depthBuffer = {};

		void ResetTransientAllocators()
		{
			uploadBufferArena.Reset();
			constantBufferArena.Reset();
		}
	};

	struct FrameContext
	{
		UINT frameIndex = 0;
		FrameResource* frame = nullptr;
		ID3D12GraphicsCommandList* commandList = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle = {};
		ID3D12Resource* backBuffer = nullptr;
	};
}