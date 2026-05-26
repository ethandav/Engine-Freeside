#pragma once

#pragma once
#include "D3D12ShadowMapRenderPassTypes.h"
#include "..\..\..\render\types\FramePacket.h"
#include "..\..\..\render\types\Handles.h"
#include "..\..\..\render\Lights.h"
#include "..\..\..\render\Camera.h"
#include "..\..\frame\D3D12FrameContext.h"
#include "..\..\types\ShadowFrameData.h"

#include <d3d12.h>

namespace efg::d3d12
{
	class D3D12GraphicsPipelineLibrary;
	class D3D12DescriptorContext;
	class D3D12MeshLibrary;
	class D3D12TextureFactory;
	class D3D12BufferFactory;

	class D3D12ShadowMapRenderPass
	{
	public:
		void Initialize(D3D12GraphicsPipelineLibrary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12TextureFactory* textureFactory, D3D12BufferFactory* bufferFactory);
		void Execute(const FrameContext& ctx, const FramePacket& scene, ShadowMapFrameData& shadowMapFrameData);
	private:
		struct ShadowMapPassResources
		{
			D3D12_GPU_VIRTUAL_ADDRESS lightViewCB = 0;
		};
		void BindPassResources(const FrameContext& ctx, ShadowMapPassResources& resources);
		void DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene);
		void DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, Freeside::MeshHandle handle, uint32_t instanceCount);

		D3D12_VIEWPORT m_viewport = {};
		D3D12_RECT m_scissorRect = {};
		GpuTexture2D m_shadowMap = {};


		D3D12GraphicsPipelineLibrary* m_pipelineLibrary = nullptr;
		D3D12DescriptorContext* m_descriptorContext = nullptr;
		D3D12MeshLibrary* m_meshLibrary = nullptr;
		D3D12TextureFactory* m_textureFactory = nullptr;
		D3D12BufferFactory* m_bufferFactory = nullptr;
	};
}