#include "..\..\..\include\d3d12\factories\D3D12PipelineFactory.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"

namespace efg::d3d12
{
	void D3D12PipelineFactory::Initialize(ID3D12Device* device)
	{
		m_device = device;
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> D3D12PipelineFactory::CreateGraphicsPipeline(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
		D3D12_THROW_IF_FAILED(m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso)));

		return pso;
	}
}