#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>

#include "D3D12Error.h"
#include "D3D12ShaderLibrary.h"

using Microsoft::WRL::ComPtr;

enum class PipelineId : uint32_t
{
	Triangle,
	Count
};

struct GraphicsPipelineState
{
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;

	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

class GraphicsPipelineLibary
{
public:
	void Initialize(ID3D12Device* device, const ShaderLibrary& shaderLibrary);
	const GraphicsPipelineState& Get(PipelineId id) const;

private:
	void CreateRootSignature(ID3D12Device* device, ID3D12RootSignature** rootSignature);
	void CreatePipelineStateObject();
	void AddGraphicsPipeline(PipelineId id, GraphicsPipelineState pipeline);
	void CreateTrianglePipeline(ID3D12Device* device, const ShaderLibrary& shaderLibrary);

	std::array<GraphicsPipelineState, static_cast<size_t>(PipelineId::Count)> m_graphicsPipelines;
};