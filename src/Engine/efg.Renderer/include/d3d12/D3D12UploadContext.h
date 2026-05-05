#pragma once
#include "D3D12Context.h"
#include "D3D12QueueFence.h"
#include <vector>

struct PendingBufferUpload
{
	ComPtr<ID3D12Resource> destination;
	ComPtr<ID3D12Resource> upload;
	UINT64 sizeInBytes = 0;
	D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_COMMON;
};

struct UploadTicket
{
	UINT64 copyFenceValue = 0;
	std::vector<PendingBufferUpload> resources = {};
};

struct UploadBatch
{
	UINT64 copyFenceValue = 0;
	std::vector<PendingBufferUpload> uploads = {};
};

class D3D12UploadContext
{
public:
	void Initialize(D3D12Context* graphicsContext);
	ID3D12CommandQueue* GetCommandQueue();
	void BeginRecording();
	void EndRecording();
	void Submit();
	void CopyBufferRegion(ID3D12Resource* buffer1, ID3D12Resource* buffer2, UINT64 sizeInBytes);
	void QueueBufferForUpload(ID3D12Resource* dest, ID3D12Resource* src, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState);
	UploadTicket FlushUploads();
	void RetireCompletedUploads();

	D3D12QueueFence copyfence = {};
	size_t queueSize = 0;
	size_t pendingBatchesSize = 0;
private:
	void CreateCopyCommandAllocator();
	void CreateCopyQueue();
	void CreateCopyCommandList();
	void CreateCopyQueueFence();

	D3D12Context* m_graphicsContext = nullptr;
	ComPtr<ID3D12CommandQueue> m_copyQueue;
	ComPtr<ID3D12CommandAllocator> m_copyCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_copyCommandList;
	std::vector<PendingBufferUpload> m_queuedBufferUploads = {};
	std::vector<UploadBatch> m_pendingBatches = {};
};