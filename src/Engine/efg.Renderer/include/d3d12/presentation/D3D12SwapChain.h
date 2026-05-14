#pragma once

#include "..\descriptors\D3D12DescriptorAllocation.h"

#include <wrl.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <array>

namespace efg::d3d12
{
	class D3D12Context;
	class D3D12DirectCommandContext;
	class D3D12DescriptorContext;

	class D3D12SwapChain
	{
	public:
		void Initialize(D3D12Context* graphicsContext, D3D12DirectCommandContext* commandContext, D3D12DescriptorContext* descriptorContext);
		void CreateSwapChain(void* nativeWindowHandle, uint32_t width, uint32_t height);
		uint64_t GetFrameIndex();
		ID3D12Resource* GetCurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle() const;
		void UpdateFrameIndex();
		void Present();
		void CreateBackBufferViews();

		static constexpr uint32_t FrameCount = 2;
	private:

		uint64_t m_frameIndex = 0;
		D3D12Context* m_graphicsContext = nullptr;
		D3D12DirectCommandContext* m_commandContext = nullptr;
		D3D12DescriptorContext* m_descriptorContext = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_backBuffers[FrameCount];
		std::array<DescriptorAllocation, FrameCount> m_backBufferHandles;
	};
}