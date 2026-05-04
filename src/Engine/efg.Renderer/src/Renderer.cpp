#include "..\include\Renderer.h"

void Renderer::Initialize(const RendererDesc& desc)
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
    m_swapChain.Initialize(&m_graphicsContext, &m_commandContext, &m_descriptorContext);
    m_descriptorContext.Initialize(&m_graphicsContext);
    m_frameSync.Initialize(&m_graphicsContext);


    m_swapChain.CreateSwapChain(desc.nativeWindowHandle, desc.width, desc.height);
    m_descriptorContext.CreateRTVDescriptorHeap(2);
    m_swapChain.CreateRenderTargetViews();

    m_frameSync.CreateFence();
    m_frameSync.WaitForGPU(m_commandContext.GetCommandQueue());

    m_shaderLibrary.Initialize();
    m_graphicsPipelineLibrary.Initialize(&m_graphicsContext, m_shaderLibrary);
}

MeshHandle Renderer::UploadMesh(const MeshData& mesh)
{
    ID3D12GraphicsCommandList* list = m_commandContext.GetCommandList();
    MeshHandle handle = m_meshLibrary.RegisterMesh(mesh);
    m_commandContext.BeginRecording();
    GpuBuffer vertexBuffer = m_bufferFactory.CreateStaticBuffer(m_graphicsContext.GetDevice(), list, mesh.vertices.data(), (mesh.vertices.size() * sizeof(Vertex)), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    GpuBuffer indexBuffer = m_bufferFactory.CreateStaticBuffer(m_graphicsContext.GetDevice(), list, mesh.indices.data(), (mesh.indices.size() * sizeof(uint32_t)), D3D12_RESOURCE_STATE_INDEX_BUFFER);
    m_meshLibrary.SetVertexBuffer(handle, vertexBuffer);
    m_meshLibrary.SetIndexBuffer(handle, indexBuffer);
    m_commandContext.EndRecording();
    m_frameSync.WaitForGPU(m_commandContext.GetCommandQueue());
    return handle;
}

void Renderer::DrawMesh(MeshHandle handle)
{
    ID3D12GraphicsCommandList* list = m_commandContext.GetCommandList();
    const GpuMesh& mesh = m_meshLibrary.Get(handle);
    const GraphicsPipelineState& pipeline = m_graphicsPipelineLibrary.Get(PipelineId::Triangle);

    list->SetGraphicsRootSignature(pipeline.rootSignature.Get());
    list->SetPipelineState(pipeline.pipelineState.Get());
    list->IASetPrimitiveTopology(pipeline.primitiveTopology);
    list->IASetVertexBuffers(0,1,&mesh.vertexBufferView);

    if (mesh.indexCount > 0)
    {
        list->IASetIndexBuffer(&mesh.indexBufferView);
        list->DrawIndexedInstanced(mesh.indexCount,1,0,0,0);
    }
    else
    {
        list->DrawInstanced(mesh.vertexCount,1,0,0);
    }
}
