#pragma once
#include <memory>
#include <string>

#include "MeshData.h"
#include "Camera.h"
#include "Lights.h"

class IRendererBackend;
class RenderThread; 

struct RendererDesc
{
	void* nativeWindowHandle = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;
};

struct Material
{
	efg::Vec4 baseColor = efg::Vec4(0.8f, 0.8f, 0.8f, 0.0f);
	efg::Vec4 specular = efg::Vec4(1.0f, 64.0f, 0.0f, 0.0f);
};

struct RenderObject
{
	efg::MeshHandle mesh;
	efg::Mat4 world = efg::Mat4::Identity();
	efg::Mat4 initialTransform = efg::Mat4::Identity();
	Material material = {};
	std::wstring name = L"Render Object";
};

struct FramePacket
{
	uint64_t frameId = 0;
	efg::Camera camera = {};
	efg::Lights::Directional directionalLight = {};
	std::vector<RenderObject> renderObjects = {};
	std::vector<efg::Lights::Point> pointLights = {};
};

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Initialize(const RendererDesc& desc);
	float GetRendererAspectRatio();
	void Shutdown();
	void SubmitFrame(FramePacket sceneRenderData);
	efg::MeshHandle CreateMesh(const efg::MeshData& mesh);
private:
	std::unique_ptr<IRendererBackend> m_backend;
	std::unique_ptr<RenderThread> m_renderThread;
	RendererDesc m_rendererDesc = {};
};