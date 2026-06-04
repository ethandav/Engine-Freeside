#pragma once
#include "..\..\render\IRendererBackend.h"
#include "..\..\render\RenderQueue.h"
#include "..\..\render\types\Handles.h"
#include "..\..\render\types\MeshTypes.h"
#include "..\..\render\types\FramePacket.h"
#include "..\..\render\types\RendererDesc.h"
#include "..\frame\D3D12RenderTargets.h"
#include "..\systems\D3D12ShadowSystem.h"
#include "..\systems\D3D12DeviceSystem.h"
#include "..\systems\D3D12ResourceSystem.h"
#include "..\systems\D3D12PipelineSystem.h"
#include "..\systems\D3D12FrameSystem.h"
#include "..\systems\D3D12RenderPassSystem.h"

namespace efg::d3d12
{
	class D3D12RendererBackend final : public IRendererBackend
	{
	public:
		void Initialize(const Freeside::RendererDesc& desc) override;
		void Shutdown() override;
		void Render(const FramePacket& scene) override;
		Freeside::MeshHandle CreateMesh(const Freeside::MeshDesc& mesh) override;
		Freeside::MaterialHandle RegisterMaterial(const Freeside::MaterialDesc& mat) override;
		Freeside::TextureHandle CreateMaterialTexture2d(const Freeside::TextureDesc& texture) override;

	private:
		void CreateRenderTargets(uint32_t width, uint32_t height);
		void InitializeD3D12Systems(const Freeside::RendererDesc& desc);

		D3D12DeviceSystem m_device = {};
		D3D12ResourceSystem m_resources = {};
		D3D12PipelineSystem m_pipeline = {};
		D3D12FrameSystem m_frame = {};
		D3D12RenderPassSystem m_passes = {};
		RenderTargets m_renderTargets = {};
		RenderQueue m_renderQueue = {};
	};
}