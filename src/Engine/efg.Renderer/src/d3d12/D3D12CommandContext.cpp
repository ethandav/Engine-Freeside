#include "..\..\include\d3d12\D3D12CommandContext.h"
#include "..\..\include\d3d12\D3D12Error.h"

#include <d3dx12.h>

void D3D12CommandContext::Initialize(D3D12Context* context)
{
    m_graphicsContext = context;
    CreateCommandObjects();
}

ID3D12GraphicsCommandList* D3D12CommandContext::GetCommandList()
{
    return m_commandList.Get();
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

ComPtr<ID3D12CommandAllocator> D3D12CommandContext::CreateCommandAllocator()
{
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
    return commandAllocator;
}

void D3D12CommandContext::CreateCommandList()
{
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
    D3D12_THROW_IF_FAILED(m_commandList->Close());
}

void D3D12CommandContext::CreateCommandObjects()
{
    m_commandAllocator = CreateCommandAllocator();
	CreateCommandQueue();
    CreateCommandList();
    m_commandAllocator.Reset();
}


void D3D12CommandContext::BeginRecording(ID3D12CommandAllocator* commandAllocator)
{
    D3D12_THROW_IF_FAILED(commandAllocator->Reset());
    D3D12_THROW_IF_FAILED(m_commandList->Reset(commandAllocator, nullptr));
}

void D3D12CommandContext::EndRecording()
{
    D3D12_THROW_IF_FAILED(m_commandList->Close());
}

void D3D12CommandContext::ResourceBarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource, before, after);
    m_commandList->ResourceBarrier(1, &barrier);
}

void D3D12CommandContext::Execute()
{
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void D3D12CommandContext::SetViewportAndScissor(const D3D12_VIEWPORT& m_viewport, const D3D12_RECT& m_scissorRect)
{
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);
}

void D3D12CommandContext::SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& renderTargetDescriptorHandle)
{
    m_commandList->OMSetRenderTargets(1, &renderTargetDescriptorHandle, FALSE, nullptr);
}

void D3D12CommandContext::ClearRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, const float clearColor[])
{
    m_commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
}