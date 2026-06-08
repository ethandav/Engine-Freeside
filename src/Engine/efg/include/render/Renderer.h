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
		MeshHandle CreateMesh(const MeshDesc& mesh);
		MaterialHandle RegisterMaterial(const MaterialDesc& mat);
		TextureHandle CreateMaterialTexture2d(const TextureDesc& texture);
		TextureHandle CreateTextureCube(const std::array<TextureDesc, 6> faces);
	private:
		std::unique_ptr<efg::d3d12::IRendererBackend> m_backend;
		std::unique_ptr<efg::RenderThread> m_renderThread;
		RendererDesc m_rendererDesc = {};
	};
}