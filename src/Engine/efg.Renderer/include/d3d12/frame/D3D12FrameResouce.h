#pragma once
#include "..\resources\D3D12GpuArena.h"

#include <wrl.h>
#include <d3d12.h>

namespace efg::d3d12
{
	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		uint64_t fenceValue = 0;

		GpuUploadBufferArena uploadBufferArena = {};
		GpuConstantBufferArena constantBufferArena = {};
		GpuDescriptorArena descriptorArena = {};

		void ResetTransientAllocators()
		{
			uploadBufferArena.Reset();
			constantBufferArena.Reset();
			descriptorArena.Reset();
		}
	};


}