#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <stdexcept>

#include "d3d12\D3D12Context.h"
#include "d3d12\D3D12Error.h"
#include "d3d12\D3D12GraphicsPipelineLibrary.h"
#include "d3d12\D3D12ShaderLibrary.h"
#include "d3d12\D3D12BufferFactory.h"
#include "d3d12\D3D12MeshLibrary.h"
#include "d3d12\D3D12CommandContext.h"
#include "d3d12\D3D12SwapChain.h"
#include "d3d12\D3D12FrameSynchronizer.h"
#include "MeshData.h"

using Microsoft::WRL::ComPtr;
using namespace Engine;

struct RendererDesc
{
	void* nativeWindowHandle = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;
};

class Renderer
{
public:
	void Initialize(const RendererDesc& desc);
	void BeginFrame();
	void Clear();
	void EndFrame();
	MeshHandle UploadMesh(const MeshData& mesh);
	void DrawMesh(MeshHandle handle);
private:
	UINT m_rtvDescriptorSize = 0;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	D3D12Context m_graphicsContext = {};
	D3D12CommandContext m_commandContext = {};
	D3D12DescriptorContext m_descriptorContext = {};
	GraphicsPipelineLibary m_graphicsPipelineLibrary;
	ShaderLibrary m_shaderLibrary;
	BufferFactory m_bufferFactory;
	MeshLibrary m_meshLibrary;
	SwapChain m_swapChain = {};
	D3D12FrameSynchronizer m_frameSync = {};
};