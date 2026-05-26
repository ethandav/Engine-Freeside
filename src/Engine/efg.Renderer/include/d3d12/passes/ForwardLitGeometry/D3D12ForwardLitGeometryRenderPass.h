#pragma once
#include "D3D12ForwardLitGeometryTypes.h"
#include "..\..\..\render\types\FramePacket.h"
#include "..\..\..\render\types\Handles.h"
#include "..\..\..\render\Lights.h"
#include "..\..\..\render\Camera.h"
#include "..\..\frame\D3D12FrameContext.h"
#include "..\..\..\d3d12\resources\D3D12GpuTexture.h"
#include "..\..\types\ShadowFrameData.h"

#include <d3d12.h>

namespace efg::d3d12
{
	class D3D12GraphicsPipelineLibary;
	class D3D12DescriptorContext;
	class D3D12MeshLibrary;
	class D3D12MaterialLibrary;
	class D3D12MaterialTextureLibrary;
	class D3D12BufferFactory;

	class D3D12ForwardLitGeometryRenderPass
	{
	public:
		void Initialize(D3D12GraphicsPipelineLibary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12MaterialLibrary* materialLibrary, D3D12MaterialTextureLibrary* textureLibrary, D3D12BufferFactory* bufferFactory);
		void Execute(const FrameContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData);
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
		void UploadPassResources(const FrameContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources);
		void BindPassResources(const FrameContext& ctx, ForwardLitPassResources& resources);
		void UploadPointLights(const FrameContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources);
		void UploadDirectionalLights(const FrameContext& ctx, const FramePacket& scene, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources);
		void UploadFrameConstants(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources);
		void UploadShadowResources(const FrameContext& ctx, const ShadowMapFrameData& shadowMapFrameData, ForwardLitPassResources& resources);
		CameraConstants BuildCameraConstants(const Freeside::Camera& camera);
		DirectionalLightConstants BuildDirectionalLightConstants(const Freeside::Lights::Directional& light);
		void DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene);

		D3D12GraphicsPipelineLibary* m_pipelineLibrary = nullptr;
		D3D12DescriptorContext* m_descriptorContext = nullptr;
		D3D12MeshLibrary* m_meshLibrary = nullptr;
		D3D12MaterialLibrary* m_materialLibrary = nullptr;
		D3D12MaterialTextureLibrary* m_textureLibrary = nullptr;
		D3D12BufferFactory* m_bufferFactory = nullptr;
	};
}