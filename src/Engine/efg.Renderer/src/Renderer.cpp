#include "..\include\Renderer.h"

void Renderer::Initialize(const RendererDesc& desc)
{
    UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif
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

    CreateFactory();
    CreateDevice();
    CreateCommandObjects();
    CreateSwapChain(desc.nativeWindowHandle, desc.width, desc.height);
    CreateDescriptorHeaps();
    CreateRenderTargetViews();
    CreateFence();

    m_shaderLibrary.Initialize();
    m_graphicsPipelineLibrary.Initialize(m_device.Get(), m_shaderLibrary);
}

MeshHandle Renderer::UploadMesh(const MeshData& mesh)
{
    MeshHandle handle = m_meshLibrary.RegisterMesh(mesh);
    BeginUploadCommands();
    GpuBuffer vertexBuffer = m_bufferFactory.CreateStaticBuffer(m_device.Get(), m_commandList.Get(), mesh.vertices.data(), (mesh.vertices.size() * sizeof(Vertex)), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    GpuBuffer indexBuffer = m_bufferFactory.CreateStaticBuffer(m_device.Get(), m_commandList.Get(), mesh.indices.data(), (mesh.indices.size() * sizeof(uint32_t)), D3D12_RESOURCE_STATE_INDEX_BUFFER);
    m_meshLibrary.SetVertexBuffer(handle, vertexBuffer);
    m_meshLibrary.SetIndexBuffer(handle, indexBuffer);
    EndUploadCommands();
    return handle;
}

void Renderer::DrawMesh(MeshHandle handle)
{
    const GpuMesh& mesh = m_meshLibrary.Get(handle);
    const GraphicsPipelineState& pipeline = m_graphicsPipelineLibrary.Get(PipelineId::Triangle);

    m_commandList->SetGraphicsRootSignature(pipeline.rootSignature.Get());
    m_commandList->SetPipelineState(pipeline.pipelineState.Get());
    m_commandList->IASetPrimitiveTopology(pipeline.primitiveTopology);
    m_commandList->IASetVertexBuffers(0,1,&mesh.vertexBufferView);

    if (mesh.indexCount > 0)
    {
        m_commandList->IASetIndexBuffer(&mesh.indexBufferView);
        m_commandList->DrawIndexedInstanced(mesh.indexCount,1,0,0,0);
    }
    else
    {
        m_commandList->DrawInstanced(mesh.vertexCount,1,0,0);
    }
}

void Renderer::BeginUploadCommands()
{
    D3D12_THROW_IF_FAILED(m_commandAllocator->Reset());
    D3D12_THROW_IF_FAILED(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
}

void Renderer::EndUploadCommands()
{
    D3D12_THROW_IF_FAILED(m_commandList->Close());

    ID3D12CommandList* lists[] =
    {
        m_commandList.Get()
    };

    m_commandQueue->ExecuteCommandLists(_countof(lists), lists);
    WaitForGPU();
}