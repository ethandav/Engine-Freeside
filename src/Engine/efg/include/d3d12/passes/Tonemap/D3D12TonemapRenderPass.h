#pragma once
#include "D3D12TonemapTypes.h"
#include "..\..\frame\D3D12PassContext.h"
#include "..\..\..\render\types\FramePacket.h"
#include "..\..\..\render\types\Handles.h"
#include "..\..\frame\D3D12RenderTargets.h"

#include <d3d12.h>

namespace efg::d3d12
{
	class D3D12TonemapRenderPass
	{
	public:
		void Execute(const D3D12PassContext& ctx, const GpuTexture2D& sceneColor);
	private:
		void UploadPassResources(D3D12PassContext& ctx, const FramePacket& scene);
		void BindPassResources(D3D12PassContext& ctx);
		void UploadPointLights(D3D12PassContext& ctx, const FramePacket& scene);
		void UploadDirectionalLights(D3D12PassContext& ctx, const FramePacket& scene);
		void UploadFrameConstants(D3D12PassContext& ctx, const FramePacket& scene);
		void DrawAllRenderObjects(D3D12PassContext& ctx, const FramePacket& scene);
	};
}