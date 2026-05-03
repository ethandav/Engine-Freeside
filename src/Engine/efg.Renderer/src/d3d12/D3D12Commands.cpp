#include "..\..\include\Renderer.h"

void CreateCommandQueue(ID3D12Device* device, ID3D12CommandQueue** commandQueue)
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    D3D12_THROW_IF_FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue)));
}

void CreateCommandAllocator(ID3D12Device* device, ID3D12CommandAllocator** commandAllocator)
{
    D3D12_THROW_IF_FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator)));
}

void CreateCommandList(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator, ID3D12GraphicsCommandList** commandList)
{
    D3D12_THROW_IF_FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(commandList)));
    D3D12_THROW_IF_FAILED((*commandList)->Close());
}

void Renderer::CreateCommandObjects()
{
	CreateCommandQueue(m_graphicsContext.GetDevice(), m_commandQueue.GetAddressOf());
    CreateCommandAllocator(m_graphicsContext.GetDevice(), m_commandAllocator.GetAddressOf());
    CreateCommandList(m_graphicsContext.GetDevice(), m_commandAllocator.Get(), m_commandList.GetAddressOf());
}