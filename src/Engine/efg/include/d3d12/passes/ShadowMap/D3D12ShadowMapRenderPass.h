#pragma once

#pragma once
#include "D3D12ShadowMapRenderPassTypes.h"
#include "..\..\..\render\types\FramePacket.h"
#include "..\..\..\render\types\Handles.h"
#include "..\..\..\render\Lights.h"
#include "..\..\..\render\Camera.h"
#include "..\..\frame\D3D12FrameContext.h"
#include "..\..\types\ShadowFrameData.h"
#include "..\..\frame\D3D12PassContext.h"

#include <d3d12.h>

namespace efg::d3d12
{
	class D3D12ShadowMapRenderPass
	{
	public:
		void Execute(D3D12PassContext& ctx, const FramePacket& scene, ShadowMapFrameData& shadowMapFrameData);
	private:
		struct ShadowMapPassResources
		{
			D3D12_GPU_VIRTUAL_ADDRESS lightViewCB = 0;
			D3D12_GPU_VIRTUAL_ADDRESS pointShadowCB = 0;
		};
		void BindPassResources(D3D12PassContext& ctx, ShadowMapPassResources& resources);
		void DrawAllRenderObjects(D3D12PassContext& ctx, const FramePacket& scene);
		void DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, Freeside::MeshHandle handle, uint32_t instanceCount);

		D3D12_VIEWPORT m_viewport = {};
		D3D12_RECT m_scissorRect = {};
		GpuTexture2D m_shadowMap = {};
	};
}