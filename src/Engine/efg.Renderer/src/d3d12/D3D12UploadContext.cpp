#include "..\..\include\d3d12\D3D12UploadContext.h"
#include "..\..\include\d3d12\D3D12Error.h"

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
	queueSize = m_queuedBufferUploads.size();
}

UploadTicket D3D12UploadContext::FlushUploads()
{
	UploadTicket ticket = {};
	if (m_queuedBufferUploads.size() > 0)
	{
		BeginRecording();

		for (const PendingBufferUpload& upload : m_queuedBufferUploads)
		{
			CopyBufferRegion(upload.destination.Get(), upload.upload.Get(), upload.sizeInBytes);
		}

		EndRecording();
		Submit();
		UINT64 copyFenceValue = copyfence.Signal(m_copyQueue.Get());
		UploadBatch batch = {};
		batch.copyFenceValue = copyFenceValue;
		batch.uploads = std::move(m_queuedBufferUploads);
		ticket.copyFenceValue = copyFenceValue;
		ticket.resources = batch.uploads;
		m_pendingBatches.push_back(std::move(batch));
		pendingBatchesSize = m_pendingBatches.size();
		m_queuedBufferUploads.clear();
	}

	return ticket;
}

void D3D12UploadContext::RetireCompletedUploads()
{
	const UINT64 completed = copyfence.GetCompletedValue();
	std::erase_if(m_pendingBatches, [completed](const UploadBatch& batch){ return completed >= batch.copyFenceValue;});
	pendingBatchesSize = m_pendingBatches.size();
}
