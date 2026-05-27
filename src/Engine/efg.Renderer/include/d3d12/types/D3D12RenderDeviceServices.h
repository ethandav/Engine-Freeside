#pragma once

#include "..\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\descriptors\D3D12DescriptorContext.h"
#include "..\factories\D3D12BufferFactory.h"

namespace efg::d3d12
{
    struct D3D12RenderServices
    {
        D3D12GraphicsPipelineLibrary* pipelines = nullptr;
        D3D12DescriptorContext* descriptors = nullptr;
        D3D12BufferFactory* buffers = nullptr;
    };
}