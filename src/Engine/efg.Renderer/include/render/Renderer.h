#pragma once
#include <memory>
#include <string>

#include "RenderTypes.h"

namespace efg
{
	class IRendererBackend;
	class RenderThread;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		void Initialize(const RendererDesc& desc);
		float GetRendererAspectRatio();
		void Shutdown();
		void SubmitFrame(FramePacket sceneRenderData);
		MeshHandle CreateMesh(const MeshData& mesh);
		MaterialHandle RegisterMaterial(const MaterialDesc& mat);
	private:
		std::unique_ptr<IRendererBackend> m_backend;
		std::unique_ptr<RenderThread> m_renderThread;
		RendererDesc m_rendererDesc = {};
	};
}