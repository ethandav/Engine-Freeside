#pragma once

#include "types/RendererDesc.h"
#include "types/MeshTypes.h"
#include "types/FramePacket.h"

#include "..\..\..\Freeside.Assets\include\MaterialTypes.h"

#include <memory>
#include <string>

namespace efg::d3d12
{
	class IRendererBackend;
}
namespace efg
{
	class RenderThread;
}

namespace Freeside
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		void Initialize(const RendererDesc& desc);
		float GetRendererAspectRatio();
		void Shutdown();
		void SubmitFrame(efg::FramePacket sceneRenderData);
		void RenderImmediate(efg::FramePacket sceneRenderData);
		void StartFrame();
		void EndFrame();
		MeshHandle CreateMesh(const MeshDesc& mesh);
		MaterialHandle RegisterMaterial(const MaterialDesc& mat);
		TextureHandle CreateMaterialTexture2d(const TextureDesc& texture);
		TextureHandle CreateTextureCube(const std::array<TextureDesc, 6> faces);
#if defined(EFG_ENABLE_IMGUI)
		bool HandleNativeWindowMessage(void* hwnd, uint32_t message, uintptr_t wParam, intptr_t lParam, intptr_t& outResult);
#endif
	private:
		std::unique_ptr<efg::d3d12::IRendererBackend> m_backend;
		std::unique_ptr<efg::RenderThread> m_renderThread;
		RendererDesc m_rendererDesc = {};
	};
}