#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>

#include "D3D12Error.h"
#include "D3D12ShaderLibrary.h"
#include "D3D12Context.h"

using Microsoft::WRL::ComPtr;

enum class PipelineId : uint32_t
{
	ForwardLitGeometry,
	Count
};

enum class ForwardLitRootParameter : UINT
{
	Camera = 0,
	Object = 1,
	DirectionalLight = 2,
	Material = 3,
	PointLightConstants = 4,
	PointLightsSrv = 5,
	InstanceData = 6
};

struct GraphicsPipelineState
{
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;

	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

class D3D12GraphicsPipelineLibary
{
public:
	void Initialize(D3D12Context* context, const D3D12ShaderLibrary& shaderLibrary);
	const GraphicsPipelineState& Get(PipelineId id) const;

private:
	void CreateRootSignature(ID3D12RootSignature** rootSignature);
	void CreatePipelineStateObject();
	void AddGraphicsPipeline(PipelineId id, GraphicsPipelineState pipeline);
	void CreateForwarLitGeometryPipeline(const D3D12ShaderLibrary& shaderLibrary);

	D3D12Context* m_graphicsContext = nullptr;
	std::array<GraphicsPipelineState, static_cast<size_t>(PipelineId::Count)> m_graphicsPipelines;
};