#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <stdexcept>

#include "d3d12\D3D12Context.h"
#include "..\include\d3d12\D3D12Error.h"
#include "..\include\d3d12\D3D12GraphicsPipelineLibrary.h"
#include "..\include\d3d12\D3D12ShaderLibrary.h"
#include "..\include\d3d12\D3D12BufferFactory.h"
#include "..\include\d3d12\D3D12MeshLibrary.h"
#include "..\include\MeshData.h"
#include "d3d12\D3D12CommandContext.h"

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
	void CreateSwapChain(void* nativeWindowHandle, uint32_t width, uint32_t height);
	void CreateDescriptorHeaps();
	void CreateRenderTargetViews();
	void CreateFence();
	void WaitForGPU();

	static const UINT FrameCount = 2;

	UINT m_rtvDescriptorSize = 0;
	UINT m_frameIndex = 0;
	HANDLE m_fenceEvent;
	UINT64 m_fenceValue;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	ComPtr<ID3D12Fence> m_fence;

	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];

	D3D12Context m_graphicsContext = {};
	D3D12CommandContext m_commandContext = {};
	GraphicsPipelineLibary m_graphicsPipelineLibrary;
	ShaderLibrary m_shaderLibrary;
	BufferFactory m_bufferFactory;
	MeshLibrary m_meshLibrary;
};