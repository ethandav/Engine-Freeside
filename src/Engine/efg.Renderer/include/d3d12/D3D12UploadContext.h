#pragma once
#include "D3D12Context.h"
#include "D3D12QueueFence.h"
#include "D3D12ResourceFactory.h"
#include <vector>

namespace efg::d3d12
{
	struct PendingBufferUpload
	{
		ComPtr<ID3D12Resource> destination;
		ComPtr<ID3D12Resource> upload;
		UINT64 sizeInBytes = 0;
		D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_COMMON;
	};

	struct PendingTextureUpload
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> destination;
		Microsoft::WRL::ComPtr<ID3D12Resource> upload;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	};

	struct UploadedResource
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> destination;
		D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_COMMON;
	};

	struct UploadTicket
	{
		UINT64 copyFenceValue = 0;
		std::vector<UploadedResource> resources = {};
	};

	struct UploadBatch
	{
		UINT64 copyFenceValue = 0;
		std::vector<UploadedResource> resources = {};
		std::vector<PendingBufferUpload> bufferUploads = {};
		std::vector<PendingTextureUpload> textureUploads = {};
	};

	class D3D12UploadContext
	{
	public:
		void Initialize(D3D12Context* graphicsContext, D3D12ResourceFactory* resourceFactory);
		ID3D12CommandQueue* GetCommandQueue();
		void BeginRecording();
		void EndRecording();
		void Submit();

		void QueueBufferUpload(ID3D12Resource* dest, const void* data, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState);
		void QueueTextureUpload(ID3D12Resource* destination, const void* sourceData, const D3D12_RESOURCE_DESC& destinationDesc, uint32_t sourceRowPitch, D3D12_RESOURCE_STATES finalState);
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
		D3D12ResourceFactory* m_resourceFactory = nullptr;
		ComPtr<ID3D12CommandQueue> m_copyQueue;
		ComPtr<ID3D12CommandAllocator> m_copyCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_copyCommandList;
		std::vector<PendingBufferUpload> m_queuedBufferUploads = {};
		std::vector<PendingTextureUpload> m_queuedTextureUploads = {};
		std::vector<UploadBatch> m_pendingBatches = {};
	};
}