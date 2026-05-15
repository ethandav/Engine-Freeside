#pragma once

#pragma once
#include "..\..\..\include\d3d12\passes\D3D12ShadowMapRenderPassTypes.h"
#include "..\..\render\types\FramePacket.h"
#include "..\..\render\types\Handles.h"
#include "..\..\render\Lights.h"
#include "..\..\render\Camera.h"
#include "..\frame\D3D12FrameContext.h"

#include <d3d12.h>

namespace efg::d3d12
{
	class D3D12GraphicsPipelineLibary;
	class D3D12DescriptorContext;
	class D3D12MeshLibrary;
	class D3D12MaterialLibrary;
	class D3D12TextureLibrary;
	class D3D12BufferFactory;

	class D3D12ShadowMapRenderPass
	{
	public:
		void Initialize(D3D12GraphicsPipelineLibary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12MaterialLibrary* materialLibrary, D3D12TextureLibrary* textureLibrary, D3D12BufferFactory* bufferFactory);
		void Execute(const FrameContext& ctx, const FramePacket& scene);
	private:
		struct ShadowMapPassResources
		{
			D3D12_GPU_VIRTUAL_ADDRESS lightViewCB = 0;
		};
		void UploadPassResources(const FrameContext& ctx, const FramePacket& scene, ShadowMapPassResources& resources);
		void BindPassResources(const FrameContext& ctx, ShadowMapPassResources& resources);
		void UploadFrameConstants(const FrameContext& ctx, const FramePacket& scene, ShadowMapPassResources& resources);
		LightViewConstants BuildLightViewConstants(const Freeside::Lights::Directional& dirLight);
		void DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene);
		void DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, Freeside::MeshHandle handle, uint32_t instanceCount);

		D3D12GraphicsPipelineLibary* m_pipelineLibrary = nullptr;
		D3D12DescriptorContext* m_descriptorContext = nullptr;
		D3D12MeshLibrary* m_meshLibrary = nullptr;
		D3D12MaterialLibrary* m_materialLibrary = nullptr;
		D3D12TextureLibrary* m_textureLibrary = nullptr;
		D3D12BufferFactory* m_bufferFactory = nullptr;
	};
}