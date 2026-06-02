#pragma once
#include "..\..\render\IRendererBackend.h"
#include "..\..\render\RenderQueue.h"
#include "..\..\render\types\Handles.h"
#include "..\..\render\types\MeshTypes.h"
#include "..\..\render\types\FramePacket.h"
#include "..\frame\D3D12FrameContext.h"
#include "..\frame\D3D12FrameResouce.h"
#include "..\frame\D3D12FrameConfig.h"
#include "..\frame\D3D12RenderTargets.h"
#include "..\commands\D3D12UploadTypes.h"
#include "..\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\libraries\D3D12ShaderLibrary.h"
#include "..\factories\D3D12PipelineFactory.h"
#include "..\factories\D3D12RootSignatureFactory.h"
#include "..\passes\ForwardLitGeometry\D3D12ForwardLitGeometryRenderPass.h"
#include "..\passes\Skybox\D3D12SkyboxRenderPass.h"
#include "..\passes\ShadowMap\D3D12ShadowMapRenderPass.h"
#include "..\systems\D3D12ShadowSystem.h"

#include "..\systems\D3D12DeviceSystem.h"
#include "..\systems\D3D12ResourceSystem.h"


namespace efg::d3d12
{
	class D3D12RendererBackend final : public IRendererBackend
	{
	public:
		void Initialize(const Freeside::RendererDesc& desc) override;
		void Shutdown() override;
		void Render(const FramePacket& scene) override;
		Freeside::MeshHandle CreateMesh(const Freeside::MeshData& mesh) override;
		Freeside::MaterialHandle RegisterMaterial(const Freeside::MaterialDesc& mat) override;

	private:
		void CreateViewportAndScissor(uint32_t width, uint32_t height);
		void CreateRenderTargets(uint32_t width, uint32_t height);
		void InitializeD3D12Systems(const Freeside::RendererDesc& desc);
		void CreateFrameResources();
		void DestroyFrameResources();
		FrameContext BeginFrame();
		void ProcessUploads();
		void RecordUploadedResourceTransitions(const UploadTicket& ticket);
		void EndFrame(const FrameContext& ctx);
		void RecordBackBufferSetup(const FrameContext& ctx);

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;

		D3D12DeviceSystem m_device = {};
		D3D12ResourceSystem m_resources = {};

		D3D12GraphicsPipelineLibrary m_graphicsPipelineLibrary = {};
		D3D12ShaderLibrary m_shaderLibrary = {};
		D3D12PipelineFactory m_pipelineFactory = {};
		D3D12RootSignatureFactory m_rootSignatureFactory = {};
		D3D12ShadowSystem m_shadowSystem = {};
		D3D12RenderServices m_renderServices = {};
		D3D12RenderResourcesLibraries m_renderResources = {};

		RenderTargets m_renderTargets = {};

		D3D12ForwardLitGeometryRenderPass m_forwarLitGeometryRenderPass = {};
		D3D12ShadowMapRenderPass m_shadowMapRenderPass = {};
		D3D12SkyboxRenderPass m_skyboxRenderPass = {};

		std::array<FrameResource, NumFramesInFlight> m_frameResources = {};

		RenderQueue m_renderQueue = {};
	};
}