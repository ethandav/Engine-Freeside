#pragma once

#include "..\types\D3D12PipelineTypes.h"

#include <cstdint>
#include <array>

namespace efg::d3d12
{
	class D3D12DeviceContext;

	class D3D12GraphicsPipelineLibrary
	{
	public:
		const GraphicsPipelineState& GetPipelineById(PipelineId id) const;
		void AddGraphicsPipeline(PipelineId id, GraphicsPipelineState pipeline);
	private:
		std::array<GraphicsPipelineState, static_cast<size_t>(PipelineId::Count)> m_graphicsPipelines;
	};
}