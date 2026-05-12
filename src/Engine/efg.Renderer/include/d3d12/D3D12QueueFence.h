#pragma once

#include "D3D12Context.h"

namespace efg::d3d12
{
	class D3D12QueueFence
	{
	public:
		void Initialize(D3D12Context* graphicsContext);
		void CreateFence(UINT64 fenceValue);
		void WaitForGPU(ID3D12CommandQueue* queue);
		UINT64 Signal(ID3D12CommandQueue* queue);
		void WaitForCPU(UINT64 value);
		void WaitForQueue(ID3D12CommandQueue* queue, UINT64 value);
		UINT64 GetCompletedValue() const;

	private:
		D3D12Context* m_graphicsContext = nullptr;
		ComPtr<ID3D12Fence> m_fence;
		HANDLE m_fenceEvent = 0;
		UINT64 m_fenceValue = 0;
		UINT64 m_nextFenceValue = 0;
	};
}