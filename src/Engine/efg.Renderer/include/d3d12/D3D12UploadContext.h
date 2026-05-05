#pragma once
#include "D3D12Context.h"
#include "D3D12QueueFence.h"

class D3D12UploadContext
{
public:
	void Initialize(D3D12Context* graphicsContext);
	ID3D12CommandQueue* GetCommandQueue();
	void BeginRecording();
	void EndRecording();
	void Submit();
	void CopyBufferRegion(ID3D12Resource* buffer1, ID3D12Resource* buffer2, UINT64 sizeInBytes);

	D3D12QueueFence copyfence = {};
private:
	void CreateCopyCommandAllocator();
	void CreateCopyQueue();
	void CreateCopyCommandList();
	void CreateCopyQueueFence();

	D3D12Context* m_graphicsContext = nullptr;
	ComPtr<ID3D12CommandQueue> m_copyQueue;
	ComPtr<ID3D12CommandAllocator> m_copyCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_copyCommandList;
};