#pragma once

#include <wrl.h>
#include <d3d12.h>

namespace efg::d3d12
{
	class D3D12PipelineFactory
	{
	public:
		void Initialize(ID3D12Device* device);
		Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const;
	private:
		ID3D12Device* m_device = nullptr;
	};
}