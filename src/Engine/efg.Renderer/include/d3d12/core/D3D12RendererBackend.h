#pragma once
#include "..\..\render\IRendererBackend.h"
#include "..\..\render\RenderQueue.h"
#include "..\..\render\types\Handles.h"
#include "..\..\render\types\MeshTypes.h"
#include "..\..\render\types\FramePacket.h"
#include "..\..\render\types\RendererDesc.h"
#include "..\frame\D3D12RenderTargets.h"
#include "..\passes\ForwardLitGeometry\D3D12ForwardLitGeometryRenderPass.h"
#include "..\passes\Skybox\D3D12SkyboxRenderPass.h"
#include "..\passes\ShadowMap\D3D12ShadowMapRenderPass.h"
#include "..\systems\D3D12ShadowSystem.h"
#include "..\systems\D3D12DeviceSystem.h"
#include "..\systems\D3D12ResourceSystem.h"
#include "..\systems\D3D12PipelineSystem.h"
#include "..\systems\D3D12FrameSystem.h"

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
		void CreateRenderTargets(uint32_t width, uint32_t height);
		void InitializeD3D12Systems(const Freeside::RendererDesc& desc);

		D3D12DeviceSystem m_device = {};
		D3D12ResourceSystem m_resources = {};
		D3D12PipelineSystem m_pipeline = {};
		D3D12FrameSystem m_frame = {};

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