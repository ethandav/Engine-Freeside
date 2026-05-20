#include "..\..\..\include\d3d12\commands\D3D12CommandContext.h"
#include "..\..\..\include\d3d12\core\D3D12Context.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"

#include <d3dx12.h>

namespace efg::d3d12
{
    void D3D12DirectCommandContext::Initialize(D3D12Context* context)
    {
        m_graphicsContext = context;
        CreateCommandObjects();
    }

    ID3D12GraphicsCommandList* D3D12DirectCommandContext::GetDirectCommandList()
    {
        return m_directCommandList.Get();
    }

    ID3D12CommandQueue* D3D12DirectCommandContext::GetDirectCommandQueue()
    {
        return m_directQueue.Get();
    }

    void D3D12DirectCommandContext::CreateCommandQueue(ID3D12CommandQueue** queue, D3D12_COMMAND_LIST_TYPE type)
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = type;

        D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(queue)));
    }

    ComPtr<ID3D12CommandAllocator> D3D12DirectCommandContext::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
        return commandAllocator;
    }

    void D3D12DirectCommandContext::CreateCommandList(ID3D12GraphicsCommandList** list, ID3D12CommandAllocator* allocator, D3D12_COMMAND_LIST_TYPE type)
    {
        D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateCommandList(0, type, allocator, nullptr, IID_PPV_ARGS(list)));
        D3D12_THROW_IF_FAILED((*list)->Close());
        m_directCommandList->SetName(L"Direct Command List");
    }

    void D3D12DirectCommandContext::CreateCommandObjects()
    {
        m_directCommandAllocator = CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
        CreateCommandQueue(m_directQueue.GetAddressOf(), D3D12_COMMAND_LIST_TYPE_DIRECT);
        CreateCommandList(m_directCommandList.GetAddressOf(), m_directCommandAllocator.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_directCommandAllocator.Reset();
    }

    void D3D12DirectCommandContext::BeginRecording(ID3D12CommandAllocator* commandAllocator)
    {
        D3D12_THROW_IF_FAILED(commandAllocator->Reset());
        D3D12_THROW_IF_FAILED(m_directCommandList->Reset(commandAllocator, nullptr));
    }

    void D3D12DirectCommandContext::EndRecording()
    {
        D3D12_THROW_IF_FAILED(m_directCommandList->Close());
    }

    void D3D12DirectCommandContext::ResourceBarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
    {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource, before, after);
        m_directCommandList->ResourceBarrier(1, &barrier);
    }

    void D3D12DirectCommandContext::ExecuteDirect()
    {
        ID3D12CommandList* ppCommandLists[] = { m_directCommandList.Get() };
        m_directQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    void D3D12DirectCommandContext::SetViewportAndScissor(const D3D12_VIEWPORT& m_viewport, const D3D12_RECT& m_scissorRect)
    {
        m_directCommandList->RSSetViewports(1, &m_viewport);
        m_directCommandList->RSSetScissorRects(1, &m_scissorRect);
    }

    void D3D12DirectCommandContext::SetRenderTarget(uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* renderTargetDescriptorHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
    {
        m_directCommandList->OMSetRenderTargets(numDescriptors, renderTargetDescriptorHandle, FALSE, dsvHandle);
    }

    void D3D12DirectCommandContext::SetGraphicsRootConstantBufferView(const uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
    {
        m_directCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, bufferLocation);
    }

    void D3D12DirectCommandContext::SetGraphicsRootShaderResourceView(const uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
    {
        m_directCommandList->SetGraphicsRootShaderResourceView(rootParameterIndex, bufferLocation);
    }

    void D3D12DirectCommandContext::SetGraphicsRootDescriptorTable(const uint32_t rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle)
    {
        m_directCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
    }

    void D3D12DirectCommandContext::SetDescriptorHeaps(uint32_t numDescriptorHeaps, ID3D12DescriptorHeap* heaps[])
    {
        m_directCommandList->SetDescriptorHeaps(numDescriptorHeaps, heaps);
    }

    void D3D12DirectCommandContext::ClearRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, const float clearColor[])
    {
        m_directCommandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
    }

    void D3D12DirectCommandContext::ClearDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, float depth, uint8_t stencil)
    {
        m_directCommandList->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, nullptr);
    }

    void D3D12DirectCommandContext::BindPipeline(const GraphicsPipelineState& pipeline)
    {
        m_directCommandList->SetGraphicsRootSignature(pipeline.rootSignature.Get());
        m_directCommandList->SetPipelineState(pipeline.pipelineState.Get());
        m_directCommandList->IASetPrimitiveTopology(pipeline.primitiveTopology);
    }

    void D3D12DirectCommandContext::DrawMeshInstanced(const GpuMesh& mesh, uint32_t instanceCount)
    {
        m_directCommandList->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);
        if (mesh.indexCount > 0)
        {
            m_directCommandList->IASetIndexBuffer(&mesh.indexBufferView);
            m_directCommandList->DrawIndexedInstanced(mesh.indexCount, instanceCount, 0, 0, 0);

        }
        else
        {
            m_directCommandList->DrawInstanced(mesh.vertexCount, instanceCount, 0, 0);
        }
    }

    void D3D12DirectCommandContext::QueueBarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
    {
        PendingBarrierTransition request = {};
        request.pResource = pResource;
        request.before = before;
        request.after = after;
        m_pendingBarrierTransitions.push_back(std::move(request));
    }

    void D3D12DirectCommandContext::FlushPendingBarrierTransitions()
    {
        for (const auto& transition : m_pendingBarrierTransitions)
        {
            ResourceBarrierTransition(transition.pResource, transition.before, transition.after);
        }
        m_pendingBarrierTransitions.clear();
    }
}