#pragma once

#include <d3d12.h>
#include <stdint.h>

namespace efg::d3d12
{
	struct DescriptorAllocation
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
		D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};
		uint32_t index = UINT32_MAX;

		bool IsValid() const
		{
			return index != UINT32_MAX;
		}
	};
}