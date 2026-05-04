#pragma once

#include "D3D12Context.h"

class D3D12FrameSynchronizer
{
public:
	void Initialize(D3D12Context* graphicsContext);
	void CreateFence(UINT64 fenceValue);
	void WaitForGPU(ID3D12CommandQueue* queue);

private:
	D3D12Context* m_graphicsContext = nullptr;
	HANDLE m_fenceEvent = 0;
	UINT64 m_fenceValue = 0;
	ComPtr<ID3D12Fence> m_fence;
};