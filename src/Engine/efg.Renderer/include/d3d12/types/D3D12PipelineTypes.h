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
        Skybox,
		Count
	};

    enum class ForwardLitRootParameter : uint32_t
    {
        Camera = 0,                    // b0
        DirectionalLightMetadata = 1,  // b2
        Material = 2,                  // b3
        PointLightMetadata = 3,        // b4
        ShadowMetadata = 4,            // b5

        InstanceData = 5,              // t0
        PointLightsSrv = 6,            // t1
        BaseColorTexture = 7,          // t2 descriptor table
        DirectionalLightsSrv = 8,      // t4
        DirectionalShadowDataSrv = 9,  // t5
        PointShadowDataSrv = 10,       // t6

        Count
    };

	struct GraphicsPipelineState
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};
}