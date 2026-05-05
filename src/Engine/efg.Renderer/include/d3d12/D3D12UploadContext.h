#pragma once
#include "D3D12Context.h"

class D3D12UploadContext
{
public:
	void Initialize(D3D12Context* graphicsContext);
private:
	void CreateCopyQueue();

	D3D12Context* m_graphicsContext = nullptr;
	ComPtr<ID3D12CommandQueue> m_copyQueue;
	ComPtr<ID3D12CommandAllocator> m_copyAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_copyCommandList;

	ComPtr<ID3D12Fence> m_copyFence;
	HANDLE m_copyFenceEvent;
	UINT64 m_nextCopyFenceValue = 1;
};