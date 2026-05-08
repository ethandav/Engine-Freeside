#pragma once
#include "D3D12Context.h"
#include "D3D12DescriptorContext.h"
#include "D3D12CommandContext.h"

class D3D12SwapChain
{
public:
	void Initialize(D3D12Context* graphicsContext, D3D12DirectCommandContext* commandContext, D3D12DescriptorContext* descriptorContext);
	void CreateSwapChain(void* nativeWindowHandle, uint32_t width, uint32_t height);
	UINT GetFrameIndex();
	ID3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle() const;
	void UpdateFrameIndex();
	void Present();
	void CreateBackBufferViews();

	static constexpr uint32_t FrameCount = 2;
private:

	UINT m_frameIndex = 0;
	D3D12Context* m_graphicsContext = nullptr;
	D3D12DirectCommandContext* m_commandContext = nullptr;
	D3D12DescriptorContext* m_descriptorContext = nullptr;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Resource> m_backBuffers[FrameCount];
	D3D12_CPU_DESCRIPTOR_HANDLE m_backBufferHandles[FrameCount] = {};
};