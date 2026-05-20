#pragma once

#include <cstdint>
#include <wrl.h>
#include <d3d12.h>

namespace efg::d3d12
{
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
		InstanceData = 6,
		DirectionalLightsSrv = 9
	};

	struct GraphicsPipelineState
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};
}