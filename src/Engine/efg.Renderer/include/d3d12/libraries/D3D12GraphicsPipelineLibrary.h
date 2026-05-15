#pragma once

#include "D3D12ShaderLibrary.h"

#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <array>

namespace efg::d3d12
{
	class D3D12Context;

	enum class PipelineId : uint32_t
	{
		ForwardLitGeometry,
		ShadowMap,
		Count
	};

	enum class ForwardLitRootParameter : uint32_t
	{
		Camera = 0,
		Shadow = 1,
		DirectionalLight = 2,
		Material = 3,
		PointLightConstants = 4,
		PointLightsSrv = 5,
		InstanceData = 6
	};

	struct GraphicsPipelineState
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};

	class D3D12GraphicsPipelineLibary
	{
	public:
		void Initialize(D3D12Context* context, const D3D12ShaderLibrary& shaderLibrary);
		void BindPipeline(ID3D12GraphicsCommandList* commandList, PipelineId pipelineId);

	private:
		const GraphicsPipelineState& Get(PipelineId id) const;
		void CreateForwardLitRootSignature(ID3D12RootSignature** rootSignature);
		void CreateShadowMapRootSignature(ID3D12RootSignature** rootSignature);
		void CreatePipelineStateObject();
		void AddGraphicsPipeline(PipelineId id, GraphicsPipelineState pipeline);
		void CreateForwarLitGeometryPipeline(const D3D12ShaderLibrary& shaderLibrary);
		void CreateShadowMapPipeline(const D3D12ShaderLibrary& shaderLibrary);

		D3D12Context* m_graphicsContext = nullptr;
		std::array<GraphicsPipelineState, static_cast<size_t>(PipelineId::Count)> m_graphicsPipelines;
	};
}