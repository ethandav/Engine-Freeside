#pragma once
#include <memory>
#include <string>

#include "MeshData.h"
#include "Camera.h"
#include "Lights.h"

class IRendererBackend;

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

struct SceneRenderData
{
	const efg::Camera* camera = nullptr;
	const efg::Lights::Directional* directionalLight = nullptr;
	const std::vector<RenderObject>* renderObjects = nullptr;
};

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Initialize(const RendererDesc& desc);
	float GetRendererAspectRatio();
	void Shutdown();
	void Render(const SceneRenderData& sceneRenderData);
	efg::MeshHandle CreateMesh(const efg::MeshData& mesh);
private:
	std::unique_ptr<IRendererBackend> m_backend;
	RendererDesc m_rendererDesc = {};
};