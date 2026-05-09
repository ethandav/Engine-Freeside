#pragma once
#include <memory>
#include <string>

#include "MeshData.h"
#include "Materials.h"
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

struct RenderObject
{
	efg::MeshHandle mesh;
	efg::MaterialHandle material;
	efg::Mat4 world = efg::Mat4::Identity();
	efg::Mat4 initialTransform = efg::Mat4::Identity();
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
	efg::MaterialHandle RegisterMaterial(const efg::MaterialDesc& mat);
private:
	std::unique_ptr<IRendererBackend> m_backend;
	std::unique_ptr<RenderThread> m_renderThread;
	RendererDesc m_rendererDesc = {};
};