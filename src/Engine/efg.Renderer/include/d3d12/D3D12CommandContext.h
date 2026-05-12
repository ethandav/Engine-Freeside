#pragma once
#include "D3D12Context.h"

#include <stdint.h>

namespace efg::d3d12
{
	class D3D12DirectCommandContext
	{
	public:
		void Initialize(D3D12Context* graphicsContext);
		void BeginRecording(ID3D12CommandAllocator* commandAllocator);
		void EndRecording();
		ID3D12GraphicsCommandList* GetDirectCommandList();
		ID3D12CommandQueue* GetDirectCommandQueue();
		ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type);
		void ResourceBarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		void ExecuteDirect();
		void ExecuteCopy();
		void SetViewportAndScissor(const D3D12_VIEWPORT& m_viewport, const D3D12_RECT& m_scissorRect);
		void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);
		void ClearRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, const float clearColor[]);
		void ClearDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, float depth, uint8_t stencil);
	private:
		void CreateCommandObjects();
		void CreateCommandList(ID3D12GraphicsCommandList** list, ID3D12CommandAllocator*, D3D12_COMMAND_LIST_TYPE type);
		void CreateCommandQueue(ID3D12CommandQueue** queue, D3D12_COMMAND_LIST_TYPE type);

		D3D12Context* m_graphicsContext = {};
		ComPtr<ID3D12CommandAllocator> m_directCommandAllocator;
		ComPtr<ID3D12CommandQueue> m_directQueue;
		ComPtr<ID3D12GraphicsCommandList> m_directCommandList;
	};
}