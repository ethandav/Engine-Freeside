#include "..\..\include\d3d12\D3D12RendererBackend.h"

void D3D12RendererBackend::Initialize(const RendererDesc& desc)
{
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(desc.width);
    m_viewport.Height = static_cast<float>(desc.height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = static_cast<LONG>(desc.width);
    m_scissorRect.bottom = static_cast<LONG>(desc.height);

    m_graphicsContext.Initialize(false);
    m_commandContext.Initialize(&m_graphicsContext);
    m_uploadContext.Initialize(&m_graphicsContext);
    m_swapChain.Initialize(&m_graphicsContext, &m_commandContext, &m_descriptorContext);
    m_descriptorContext.Initialize(&m_graphicsContext);
    m_directFence.Initialize(&m_graphicsContext);

    m_swapChain.CreateSwapChain(desc.nativeWindowHandle, desc.width, desc.height);
    m_descriptorContext.CreateRTVDescriptorHeap(NumFramesInFlight);
    m_swapChain.CreateRenderTargetViews();

    for (UINT i = 0; i < NumFramesInFlight; i++)
    {
        m_frameResources[i].commandAllocator = m_commandContext.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
    }

    m_directFence.CreateFence(0);

    m_shaderLibrary.Initialize();
    m_graphicsPipelineLibrary.Initialize(&m_graphicsContext, m_shaderLibrary);

    m_directFence.WaitForGPU(m_commandContext.GetDirectCommandQueue());
}

void D3D12RendererBackend::Shutdown()
{
    m_directFence.WaitForGPU(m_commandContext.GetDirectCommandQueue());
}

MeshHandle D3D12RendererBackend::UploadMesh(const MeshData& mesh)
{
    MeshHandle handle = m_meshLibrary.RegisterMesh(mesh);
    GpuBuffer vertexBuffer = m_bufferFactory.CreateStaticBuffer(m_graphicsContext.GetDevice(), mesh.vertices.data(), (mesh.vertices.size() * sizeof(Vertex)), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    GpuBuffer indexBuffer = m_bufferFactory.CreateStaticBuffer(m_graphicsContext.GetDevice(), mesh.indices.data(), (mesh.indices.size() * sizeof(uint32_t)), D3D12_RESOURCE_STATE_INDEX_BUFFER);
    m_meshLibrary.SetVertexBuffer(handle, vertexBuffer);
    m_meshLibrary.SetIndexBuffer(handle, indexBuffer);
    m_uploadContext.QueueBufferForUpload(vertexBuffer.resource.Get(), vertexBuffer.uploadResource.Get(), vertexBuffer.sizeInBytes, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    m_uploadContext.QueueBufferForUpload(indexBuffer.resource.Get(), indexBuffer.uploadResource.Get(), indexBuffer.sizeInBytes, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    return handle;
}

void D3D12RendererBackend::DrawMesh(MeshHandle handle)
{
    ID3D12GraphicsCommandList* list = m_commandContext.GetDirectCommandList();
    const GpuMesh& mesh = m_meshLibrary.Get(handle);
    const GraphicsPipelineState& pipeline = m_graphicsPipelineLibrary.Get(PipelineId::Triangle);

    list->SetGraphicsRootSignature(pipeline.rootSignature.Get());
    list->SetPipelineState(pipeline.pipelineState.Get());
    list->IASetPrimitiveTopology(pipeline.primitiveTopology);
    list->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);

    if (mesh.indexCount > 0)
    {
        list->IASetIndexBuffer(&mesh.indexBufferView);
        list->DrawIndexedInstanced(mesh.indexCount, 1, 0, 0, 0);
    }
    else
    {
        list->DrawInstanced(mesh.vertexCount, 1, 0, 0);
    }
}

void D3D12RendererBackend::FlushPendingUploads()
{
    UploadTicket uploadTicket = m_uploadContext.FlushUploads();
    if (uploadTicket.resources.empty())
    {
        return;
    }
    m_uploadContext.copyfence.WaitForQueue(m_commandContext.GetDirectCommandQueue(), uploadTicket.copyFenceValue);
    for (const auto& upload : uploadTicket.resources)
    {
        m_commandContext.ResourceBarrierTransition(upload.destination.Get(), D3D12_RESOURCE_STATE_COPY_DEST, upload.finalState);
    }
}
