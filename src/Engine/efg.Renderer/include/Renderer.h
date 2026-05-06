#pragma once
#include <memory>

#include "MeshData.h"
#include "Camera.h"

class IRendererBackend;

struct RendererDesc
{
	void* nativeWindowHandle = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;
};

struct RenderObject
{
	efg::MeshHandle mesh;
	efg::Mat4 world = efg::Mat4::Identity();
};

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Initialize(const RendererDesc& desc);
	void Shutdown();
	void BeginFrame(efg::Camera camera);
	void EndFrame();
	efg::MeshHandle CreateMesh(const efg::MeshData& mesh);
	void DrawMesh(efg::MeshHandle handle);
private:
	std::unique_ptr<IRendererBackend> m_backend;
};