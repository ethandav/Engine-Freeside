#pragma once
#include "D3D12UploadTypes.h"
#include "D3D12QueueFence.h"

#include "..\..\..\..\Freeside.Assets\include\MaterialTypes.h"

#include <d3d12.h>
#include <vector>
#include <wrl.h>

namespace efg::d3d12
{
	class D3D12DeviceContext;
	class D3D12ResourceFactory;

	class D3D12UploadContext
	{
	public:
		void Initialize(D3D12DeviceContext* graphicsContext, D3D12ResourceFactory* resourceFactory);
		ID3D12CommandQueue* GetCommandQueue();
		void BeginRecording();
		void EndRecording();
		void Submit();

		void QueueBufferUpload(ID3D12Resource* dest, const void* data, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState);
		void QueueTexture2DUpload(ID3D12Resource* destination, const void* sourceData, const D3D12_RESOURCE_DESC& destinationDesc, uint32_t sourceRowPitch, D3D12_RESOURCE_STATES finalState);
		void QueueTextureCubeUpload(ID3D12Resource* destination, const std::array<Freeside::TextureDesc, 6>& faces, const D3D12_RESOURCE_DESC& textureDesc, D3D12_RESOURCE_STATES finalState);
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

		D3D12DeviceContext* m_graphicsContext = nullptr;
		D3D12ResourceFactory* m_resourceFactory = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_copyQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_copyCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_copyCommandList;
		std::vector<PendingBufferUpload> m_queuedBufferUploads = {};
		std::vector<PendingTexture2DUpload> m_queuedTexture2DUploads = {};
		std::vector<PendingTextureCubeUpload> m_queuedTextureCubeUploads = {};
		std::vector<UploadBatch> m_pendingBatches = {};
	};
}