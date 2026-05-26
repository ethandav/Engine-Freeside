#include "..\..\..\include\d3d12\commands\D3D12UploadContext.h"
#include "..\..\..\include\d3d12\core\D3D12Context.h"
#include "..\..\..\include\d3d12\factories\D3D12ResourceFactory.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"
#include "..\..\..\include\d3d12\core\D3D12Pix.h"

#include <d3dx12.h>

namespace efg::d3d12
{
	void D3D12UploadContext::Initialize(D3D12Context* graphicsContext, D3D12ResourceFactory* resourceFactory)
	{
		m_graphicsContext = graphicsContext;
		m_resourceFactory = resourceFactory;
		CreateCopyCommandAllocator();
		CreateCopyQueue();
		CreateCopyCommandList();
		CreateCopyQueueFence();
	}

	ID3D12CommandQueue* D3D12UploadContext::GetCommandQueue()
	{
		return m_copyQueue.Get();
	}

	void D3D12UploadContext::BeginRecording()
	{
		D3D12_THROW_IF_FAILED(m_copyCommandAllocator->Reset());
		D3D12_THROW_IF_FAILED(m_copyCommandList->Reset(m_copyCommandAllocator.Get(), nullptr));
	}

	void D3D12UploadContext::EndRecording()
	{
		D3D12_THROW_IF_FAILED(m_copyCommandList->Close());
	}

	void D3D12UploadContext::Submit()
	{
		ID3D12CommandList* ppCommandLists[] = { m_copyCommandList.Get() };
		m_copyQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	void D3D12UploadContext::CreateCopyCommandAllocator()
	{
		D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_copyCommandAllocator)));
	}

	void D3D12UploadContext::CreateCopyQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

		D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_copyQueue)));
	}

	void D3D12UploadContext::CreateCopyCommandList()
	{
		D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_copyCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_copyCommandList)));
		D3D12_THROW_IF_FAILED(m_copyCommandList.Get()->Close());

	}

	void D3D12UploadContext::CreateCopyQueueFence()
	{
		copyfence.Initialize(m_graphicsContext);
		copyfence.CreateFence(0);
	}

	void D3D12UploadContext::QueueBufferUpload(ID3D12Resource* dest, const void* data, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState)
	{
		PendingBufferUpload request = {};
		request.destination = dest;
		request.sizeInBytes = sizeInBytes;
		request.finalState = finalState;

		request.upload = m_resourceFactory->CreateCommittedUploadBufferResource(sizeInBytes);
		void* mappedData = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		D3D12_THROW_IF_FAILED(request.upload->Map(0, &readRange, &mappedData));
		memcpy(mappedData, data, static_cast<size_t>(sizeInBytes));
		request.upload->Unmap(0, nullptr);

		m_queuedBufferUploads.push_back(std::move(request));
		queueSize = m_queuedBufferUploads.size() + m_queuedTexture2DUploads.size();
	}

	void D3D12UploadContext::QueueTexture2DUpload(ID3D12Resource* destination, const void* sourceData, const D3D12_RESOURCE_DESC& textureDesc, uint32_t sourceRowPitch, D3D12_RESOURCE_STATES finalState)
	{
		if (!destination)
		{
			throw std::runtime_error("QueueTextureUpload destination is null.");
		}

		if (!sourceData)
		{
			throw std::runtime_error("QueueTextureUpload sourceData is null.");
		}

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		UINT numRows = 0;
		UINT64 rowSizeInBytes = 0;
		UINT64 uploadBufferSize = 0;
		uint8_t* mappedData = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		const uint8_t* srcBytes = static_cast<const uint8_t*>(sourceData);

		m_graphicsContext->GetDevice()->GetCopyableFootprints(&textureDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &uploadBufferSize);
		ComPtr<ID3D12Resource> upload = m_resourceFactory->CreateCommittedUploadBufferResource(uploadBufferSize);
		D3D12_THROW_IF_FAILED(upload->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)));
		uint8_t* dstBytes = mappedData + footprint.Offset;
		const UINT64 dstRowPitch = footprint.Footprint.RowPitch;

		for (UINT row = 0; row < numRows; ++row)
		{
			std::memcpy(dstBytes + row * dstRowPitch, srcBytes + row * sourceRowPitch, sourceRowPitch);
		}

		upload->Unmap(0, nullptr);

		PendingTexture2DUpload pending = {};
		pending.destination = destination;
		pending.upload = upload;
		pending.footprint = footprint;
		pending.finalState = finalState;

		m_queuedTexture2DUploads.push_back(std::move(pending));

		queueSize = m_queuedBufferUploads.size() + m_queuedTexture2DUploads.size();

	}

	void D3D12UploadContext::QueueTextureCubeUpload(ID3D12Resource* destination, const std::array<DecodedImage, 6>& faces, const D3D12_RESOURCE_DESC& textureDesc, D3D12_RESOURCE_STATES finalState)
	{
		std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 6> footprints = {};
		std::array<UINT, 6> numRows = {};
		std::array<UINT64, 6> rowSizeInBytes = {};
		UINT64 uploadBufferSize = 0;

		m_graphicsContext->GetDevice()->GetCopyableFootprints(&textureDesc, 0, 6, 0, footprints.data(), numRows.data(), rowSizeInBytes.data(), &uploadBufferSize);

		ComPtr<ID3D12Resource> upload = m_resourceFactory->CreateCommittedUploadBufferResource(uploadBufferSize);

		uint8_t* mappedData = nullptr;
		CD3DX12_RANGE readRange(0, 0);

		D3D12_THROW_IF_FAILED(upload->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)));

		for (uint32_t face = 0; face < 6; ++face)
		{
			uint8_t* dstBytes = mappedData + footprints[face].Offset;
			const uint8_t* srcBytes = faces[face].pixels.data();

			for (UINT row = 0; row < numRows[face]; ++row)
			{
				memcpy(dstBytes + row * footprints[face].Footprint.RowPitch, srcBytes + row * faces[face].rowPitch, faces[face].rowPitch);
			}
		}

		upload->Unmap(0, nullptr);

		PendingTextureCubeUpload pending = {};
		pending.destination = destination;
		pending.upload = upload;
		pending.footprints = footprints;
		pending.finalState = finalState;

		m_queuedTextureCubeUploads.push_back(std::move(pending));
	}

	UploadTicket D3D12UploadContext::FlushUploads()
	{
		UploadTicket ticket = {};

		const bool hasBufferUploads = !m_queuedBufferUploads.empty();
		const bool hasTextureUploads = !m_queuedTexture2DUploads.empty();

		if (!hasBufferUploads && !hasTextureUploads)
		{
			return ticket;
		}

		BeginRecording();
		PIXBeginEvent(m_copyCommandList.Get(), PIX_COLOR(100, 100, 255), L"FlushUploads");

		for (const PendingBufferUpload& upload : m_queuedBufferUploads)
		{
			m_copyCommandList->CopyBufferRegion(upload.destination.Get(), 0, upload.upload.Get(), 0, upload.sizeInBytes);
			ticket.resources.push_back(UploadedResource{upload.destination, upload.finalState});
		}

		for (const PendingTexture2DUpload& upload : m_queuedTexture2DUploads)
		{
			D3D12_TEXTURE_COPY_LOCATION dst = {};
			dst.pResource = upload.destination.Get();
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = 0;
			D3D12_TEXTURE_COPY_LOCATION src = {};
			src.pResource = upload.upload.Get();
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint = upload.footprint;
			m_copyCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
			ticket.resources.push_back(UploadedResource{upload.destination, upload.finalState});
		}

		for (const PendingTextureCubeUpload& upload : m_queuedTextureCubeUploads)
		{
			for (uint32_t face = 0; face < 6; ++face)
			{
				D3D12_TEXTURE_COPY_LOCATION dst = {};
				dst.pResource = upload.destination.Get();
				dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				dst.SubresourceIndex = face;

				D3D12_TEXTURE_COPY_LOCATION src = {};
				src.pResource = upload.upload.Get();
				src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
				src.PlacedFootprint = upload.footprints[face];

				m_copyCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
			}

			ticket.resources.push_back(UploadedResource{ upload.destination, upload.finalState });
		}

		PIXEndEvent(m_copyCommandList.Get());
		EndRecording();
		Submit();

		UINT64 copyFenceValue = copyfence.Signal(m_copyQueue.Get());
		ticket.copyFenceValue = copyFenceValue;

		UploadBatch batch = {};
		batch.copyFenceValue = copyFenceValue;
		batch.resources = ticket.resources;
		batch.bufferUploads = std::move(m_queuedBufferUploads);
		batch.textureUploads = std::move(m_queuedTexture2DUploads);

		m_pendingBatches.push_back(std::move(batch));
		pendingBatchesSize = m_pendingBatches.size();
		m_queuedBufferUploads.clear();
		m_queuedTexture2DUploads.clear();
		queueSize = 0;

		return ticket;
	}

	void D3D12UploadContext::RetireCompletedUploads()
	{
		const UINT64 completed = copyfence.GetCompletedValue();
		std::erase_if(m_pendingBatches, [completed](const UploadBatch& batch) { return completed >= batch.copyFenceValue;});
		pendingBatchesSize = m_pendingBatches.size();
	}
}