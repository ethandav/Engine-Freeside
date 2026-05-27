#include "..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"

#include <stdexcept>

namespace efg::d3d12
{
    const GraphicsPipelineState& D3D12GraphicsPipelineLibrary::GetPipelineById(PipelineId id) const
    {
        const auto index = static_cast<size_t>(id);

        if (index >= m_graphicsPipelines.size())
        {
            throw std::runtime_error("Invalid PipelineId.");
        }

        return m_graphicsPipelines[index];
    }

    void D3D12GraphicsPipelineLibrary::AddGraphicsPipeline(PipelineId id, GraphicsPipelineState pipeline)
    {
        const size_t index = static_cast<size_t>(id);

        if (index >= m_graphicsPipelines.size())
        {
            throw std::runtime_error("Invalid pipeline id.");
        }

        m_graphicsPipelines[index] = std::move(pipeline);
    }
}