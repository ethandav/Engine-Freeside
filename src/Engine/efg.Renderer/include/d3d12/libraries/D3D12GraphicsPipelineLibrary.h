#pragma once

#include "D3D12ShaderLibrary.h"
#include "..\types\D3D12PipelineTypes.h"

#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <array>

namespace efg::d3d12
{
	class D3D12Context;

	class D3D12GraphicsPipelineLibrary
	{
	public:
		void Initialize(D3D12Context* context, const D3D12ShaderLibrary& shaderLibrary);
		const GraphicsPipelineState& Get(PipelineId id) const;

	private:
		void CreateForwardLitRootSignature(ID3D12RootSignature** rootSignature);
		void CreateShadowMapRootSignature(ID3D12RootSignature** rootSignature);
		void CreateSkyboxRootSignature(ID3D12RootSignature** rootSignature);
		void CreatePipelineStateObject();
		void AddGraphicsPipeline(PipelineId id, GraphicsPipelineState pipeline);
		void CreateForwarLitGeometryPipeline(const D3D12ShaderLibrary& shaderLibrary);
		void CreateShadowMapPipeline(const D3D12ShaderLibrary& shaderLibrary);
		void CreateSkyboxPipeline(const D3D12ShaderLibrary& shaderLibrary);

		D3D12Context* m_graphicsContext = nullptr;
		std::array<GraphicsPipelineState, static_cast<size_t>(PipelineId::Count)> m_graphicsPipelines;
	};
}