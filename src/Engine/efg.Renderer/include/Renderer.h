#pragma once
#include <memory>

#include "MeshData.h"

class IRendererBackend;

struct RendererDesc
{
	void* nativeWindowHandle = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;
};

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Initialize(const RendererDesc& desc);
	void Shutdown();
	void BeginFrame();
	void EndFrame();
	Engine::MeshHandle UploadMesh(const Engine::MeshData& mesh);
	void DrawMesh(Engine::MeshHandle handle);
private:
	std::unique_ptr<IRendererBackend> m_backend;
};