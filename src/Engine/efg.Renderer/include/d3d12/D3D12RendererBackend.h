#pragma once
#include "..\render\IRendererBackend.h"
#include "..\render\RenderQueue.h"
#include "..\render\ImageLoader.h"
#include "D3D12Context.h"
#include "D3D12Error.h"
#include "D3D12GraphicsPipelineLibrary.h"
#include "D3D12ShaderLibrary.h"
#include "D3D12ResourceFactory.h"
#include "D3D12BufferFactory.h"
#include "D3D12TextrureFactory.h"
#include "D3D12MeshLibrary.h"
#include "D3D12CommandContext.h"
#include "D3D12SwapChain.h"
#include "D3D12QueueFence.h"
#include "D3D12UploadContext.h"
#include "D3D12TextureLibrary.h"
#include "D3D12MaterialLibrary.h"
#include "D3D12RenderData.h"
#include "passes\D3D12ForwardLitGeometryRenderPass.h"

namespace efg::d3d12
{


	class D3D12RendererBackend final : public IRendererBackend
	{
	public:
		void Initialize(const RendererDesc& desc) override;
		void Shutdown() override;
		void Render(const FramePacket& scene) override;
		MeshHandle CreateMesh(const MeshData& mesh) override;
		MaterialHandle RegisterMaterial(const MaterialDesc& mat) override;
		TextureHandle RegisterTexture2D(const wchar_t* filename) override;

	private:
		void CreateViewportAndScissor(uint32_t width, uint32_t height);
		void InitializeD3D12Systems(const RendererDesc& desc);
		void InitializeRenderPasses();
		void CreateFrameResources(uint32_t width, uint32_t height);
		void DestroyFrameResources();
		FrameContext BeginFrame();
		void ProcessUploads();
		void RecordUploadedResourceTransitions(const UploadTicket& ticket);
		void EndFrame(const FrameContext& ctx);
		void RecordBackBufferSetup(const FrameContext& ctx);

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;

		D3D12Context m_graphicsContext = {};
		D3D12DirectCommandContext m_commandContext = {};
		D3D12DescriptorContext m_descriptorContext = {};
		D3D12UploadContext m_uploadContext = {};
		D3D12GraphicsPipelineLibary m_graphicsPipelineLibrary = {};
		D3D12ShaderLibrary m_shaderLibrary = {};
		D3D12MaterialLibrary m_materialLibrary = {};
		D3D12TextureLibrary m_textureLibrary = {};
		D3D12BufferFactory m_bufferFactory = {};
		D3D12TextureFactory m_textureFactory = {};
		D3D12ResourceFactory m_resourceFactory = {};
		D3D12MeshLibrary m_meshLibrary;
		D3D12SwapChain m_swapChain = {};
		D3D12QueueFence m_directFence = {};

		D3D12ForwardLitGeometryRenderPass m_forwarLitGeometryRenderPass = {};

		std::array<FrameResource, NumFramesInFlight> m_frameResources = {};

		RenderQueue m_renderQueue = {};
		ImageLoader m_imageLoader = {};
	};
}