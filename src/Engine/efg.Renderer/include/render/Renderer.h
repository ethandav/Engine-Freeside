#pragma once
#include <memory>
#include <string>

#include "RenderTypes.h"

class IRendererBackend;
class RenderThread; 

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Initialize(const efg::RendererDesc& desc);
	float GetRendererAspectRatio();
	void Shutdown();
	void SubmitFrame(efg::FramePacket sceneRenderData);
	efg::MeshHandle CreateMesh(const efg::MeshData& mesh);
	efg::MaterialHandle RegisterMaterial(const efg::MaterialDesc& mat);
private:
	std::unique_ptr<IRendererBackend> m_backend;
	std::unique_ptr<RenderThread> m_renderThread;
	efg::RendererDesc m_rendererDesc = {};
};