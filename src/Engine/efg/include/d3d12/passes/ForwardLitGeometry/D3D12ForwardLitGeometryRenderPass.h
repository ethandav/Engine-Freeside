#pragma once
#include "D3D12ForwardLitGeometryTypes.h"
#include "..\..\types\ShadowFrameData.h"
#include "..\..\frame\D3D12PassContext.h"
#include "..\..\..\render\types\FramePacket.h"
#include "..\..\..\render\types\Handles.h"
#include "..\..\..\render\Lights.h"
#include "..\..\..\render\Camera.h"

#include <d3d12.h>

namespace efg::d3d12
{
	class D3D12ForwardLitGeometryRenderPass
	{
	public:
		void Execute(D3D12PassContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData);
	private:
		struct ForwardLitPassResources
		{
			D3D12_GPU_VIRTUAL_ADDRESS cameraCB = 0;
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightConstantsCB = 0;
			D3D12_GPU_VIRTUAL_ADDRESS pointLightConstantsCB = 0;
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightsSRV = 0;
			D3D12_GPU_VIRTUAL_ADDRESS pointLightsSRV = 0;
			D3D12_GPU_VIRTUAL_ADDRESS directionalShadowDataSRV = 0;
			D3D12_GPU_VIRTUAL_ADDRESS pointShadowDataSRV = 0;
			D3D12_GPU_VIRTUAL_ADDRESS shadowMetadataCB = 0;
			D3D12_GPU_DESCRIPTOR_HANDLE directionalShadowMapsTable = {};
			D3D12_GPU_DESCRIPTOR_HANDLE pointShadowCubesTable = {};
			DirectionalLightConstants directionalLightConstants = {};
			PointLightConstants pointLightConstants = {};
			ShadowMetadataConstants shadowMetadata = {};
		};
		void UploadPassResources(D3D12PassContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources);
		void BindPassResources(D3D12PassContext& ctx, ForwardLitPassResources& resources);
		void UploadPointLights(D3D12PassContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources);
		void UploadDirectionalLights(D3D12PassContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources);
		void UploadFrameConstants(D3D12PassContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources);
		void UploadShadowResources(D3D12PassContext& ctx, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources);
		CameraConstants BuildCameraConstants(const Freeside::Camera& camera);
		DirectionalLightConstants BuildDirectionalLightConstants(const Freeside::Lights::Directional& light);
		void DrawAllRenderObjects(D3D12PassContext& ctx, const FramePacket& scene);
	};
}