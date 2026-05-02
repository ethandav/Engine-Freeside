#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <stdexcept>

#include "..\include\d3d12\D3D12Error.h"
#include "..\include\d3d12\D3D12GraphicsPipelineLibrary.h"
#include "..\include\d3d12\D3D12ShaderLibrary.h"

using Microsoft::WRL::ComPtr;

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
private:
	void CreateFactory();
	void GetHardwareAdapter(bool requestHighPerformanceAdapter = false);
	void CreateDevice();
	void CreateCommandObjects();
	void CreateSwapChain(void* nativeWindowHandle, uint32_t width, uint32_t height);
	void CreateDescriptorHeaps();
	void CreateRenderTargetViews();
	void CreateFence();
	void WaitForGPU();

	static const UINT FrameCount = 2;

	bool m_useWarpDevice = false;
	UINT m_rtvDescriptorSize = 0;
	UINT m_frameIndex = 0;
	HANDLE m_fenceEvent;
	UINT64 m_fenceValue;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	ComPtr<ID3D12Fence> m_fence;
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<IDXGIAdapter1> m_hardwareAdapter;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];

	GraphicsPipelineLibary m_graphicsPipelineLibrary;
	ShaderLibrary m_shaderLibrary;

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	ComPtr<ID3D12Resource> m_vertexBuffer;
};