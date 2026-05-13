#include "..\..\include\d3d12\D3D12UploadContext.h"
#include "..\..\include\d3d12\D3D12Error.h"
#include "..\..\include\d3d12\D3D12Pix.h"

namespace efg::d3d12
{
	void D3D12UploadContext::Initialize(D3D12Context* graphicsContext)
	{
		m_graphicsContext = graphicsContext;
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

	void D3D12UploadContext::CopyBufferRegion(ID3D12Resource* dest, ID3D12Resource* src, UINT64 sizeInBytes)
	{
		m_copyCommandList->CopyBufferRegion(
			dest,
			0,
			src,
			0,
			sizeInBytes
		);
	}

	void D3D12UploadContext::QueueBufferForUpload(ID3D12Resource* dest, ID3D12Resource* src, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState)
	{
		PendingBufferUpload request = {};
		request.destination = dest;
		request.upload = src;
		request.sizeInBytes = sizeInBytes;

		m_queuedBufferUploads.push_back(std::move(request));
		queueSize = m_queuedBufferUploads.size() + m_queuedTextureUploads.size();
	}

	void D3D12UploadContext::QueueTextureForUpload(ID3D12Resource* destination, ID3D12Resource* upload, const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& footprint, D3D12_RESOURCE_STATES finalState)
	{
		if (!destination)
		{
			throw std::runtime_error("QueueTextureForUpload destination is null.");
		}

		if (!upload)
		{
			throw std::runtime_error("QueueTextureForUpload upload resource is null.");
		}

		PendingTextureUpload pending = {};
		pending.destination = destination;
		pending.upload = upload;
		pending.footprint = footprint;
		pending.finalState = finalState;

		m_queuedTextureUploads.push_back(std::move(pending));

		queueSize = m_queuedBufferUploads.size() + m_queuedTextureUploads.size();

	}

	UploadTicket D3D12UploadContext::FlushUploads()
	{
		UploadTicket ticket = {};

		const bool hasBufferUploads = !m_queuedBufferUploads.empty();
		const bool hasTextureUploads = !m_queuedTextureUploads.empty();

		if (!hasBufferUploads && !hasTextureUploads)
		{
			return ticket;
		}

		BeginRecording();
		PIXBeginEvent(m_copyCommandList.Get(), PIX_COLOR(100, 100, 255), L"FlushUploads");
		for (const PendingBufferUpload& upload : m_queuedBufferUploads)
		{
			CopyBufferRegion(upload.destination.Get(), upload.upload.Get(), upload.sizeInBytes);
			ticket.resources.push_back(UploadedResource{upload.destination, upload.finalState});
		}

		for (const PendingTextureUpload& upload : m_queuedTextureUploads)
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
		PIXEndEvent(m_copyCommandList.Get());
		EndRecording();
		Submit();
		UINT64 copyFenceValue = copyfence.Signal(m_copyQueue.Get());
		ticket.copyFenceValue = copyFenceValue;
		UploadBatch batch = {};
		batch.copyFenceValue = copyFenceValue;
		batch.resources = ticket.resources;
		batch.bufferUploads = std::move(m_queuedBufferUploads);
		batch.textureUploads = std::move(m_queuedTextureUploads);
		m_pendingBatches.push_back(std::move(batch));
		pendingBatchesSize = m_pendingBatches.size();
		m_queuedBufferUploads.clear();
		m_queuedTextureUploads.clear();
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