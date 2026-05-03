#include "..\..\include\d3d12\D3D12CommandContext.h"
#include "..\..\include\d3d12\D3D12Error.h"

void D3D12CommandContext::Initialize(D3D12Context* context)
{
    m_graphicsContext = context;
    CreateCommandObjects();
}

ID3D12GraphicsCommandList* D3D12CommandContext::GetCommandList()
{
    return m_commandList.Get();
}

ID3D12CommandAllocator* D3D12CommandContext::GetCommandAllocator()
{
    return m_commandAllocator.Get();
}

ID3D12CommandQueue* D3D12CommandContext::GetCommandQueue()
{
    return m_commandQueue.Get();
}

void D3D12CommandContext::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
}

void D3D12CommandContext::CreateCommandAllocator()
{
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
}

void D3D12CommandContext::CreateCommandList()
{
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
    D3D12_THROW_IF_FAILED(m_commandList->Close());
}

void D3D12CommandContext::CreateCommandObjects()
{
	CreateCommandQueue();
    CreateCommandAllocator();
    CreateCommandList();
}


void D3D12CommandContext::BeginRecording()
{
    D3D12_THROW_IF_FAILED(m_commandAllocator->Reset());
    D3D12_THROW_IF_FAILED(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
}

void D3D12CommandContext::EndRecording()
{
    D3D12_THROW_IF_FAILED(m_commandList->Close());

    ID3D12CommandList* lists[] =
    {
        m_commandList.Get()
    };

    m_commandQueue->ExecuteCommandLists(_countof(lists), lists);
}