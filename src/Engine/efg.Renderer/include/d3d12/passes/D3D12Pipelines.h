#pragma once

#include "..\types\D3D12PipelineTypes.h"

namespace efg::d3d12
{
    class D3D12PipelineFactory;
    class D3D12RootSignatureFactory;
    class D3D12ShaderLibrary;
}

namespace efg::d3d12::ShadowMapPipeline
{
    GraphicsPipelineState CreatePipeline(const D3D12PipelineFactory& pipelineFactory, const D3D12RootSignatureFactory& rootSignatureFactory, const D3D12ShaderLibrary& shaderLibrary);
}

namespace efg::d3d12::SkyboxPipeline
{
    GraphicsPipelineState CreatePipeline(const D3D12PipelineFactory& pipelineFactory, const D3D12RootSignatureFactory& rootSignatureFactory, const D3D12ShaderLibrary& shaderLibrary);
}

namespace efg::d3d12::ForwardLitGeometryPipeline
{
    GraphicsPipelineState CreatePipeline(const D3D12PipelineFactory& pipelineFactory, const D3D12RootSignatureFactory& rootSignatureFactory, const D3D12ShaderLibrary& shaderLibrary);
}