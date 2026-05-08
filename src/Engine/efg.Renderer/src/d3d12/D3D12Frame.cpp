#include "..\..\include\d3d12\D3D12RendererBackend.h"
#include "..\..\include\Camera.h"
#include "..\..\include\ShaderConstants.h"
#include "..\..\include\d3d12\D3D12Pix.h"

FrameContext D3D12RendererBackend::BeginFrame()
{
    UINT frameIndex = m_swapChain.GetFrameIndex();
    FrameResource& frame = m_frameResources[frameIndex];
    ID3D12CommandAllocator* allocator = frame.commandAllocator.Get();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain.GetCurrentBackBufferHandle();

    m_directFence.WaitForCPU(frame.fenceValue);
    m_commandContext.BeginRecording(allocator);

    FrameContext ctx = {};
    ctx.frameIndex = frameIndex;
    ctx.frame = &frame;
    ctx.commandList = m_commandContext.GetDirectCommandList();
    ctx.backBuffer = m_swapChain.GetCurrentBackBuffer();
    ctx.backBufferHandle = m_swapChain.GetCurrentBackBufferHandle();

    return ctx;
}

void D3D12RendererBackend::UpdateFrameConstants(const FrameContext& ctx, const SceneRenderData& scene)
{
    PIXBeginEvent(PIX_COLOR(255, 255, 255), L"Update Frame Constants");
    CameraConstants cameraConstants = scene.camera->BuildCameraConstants();
    Lights::DirectionalLightConstants dirLightConstants = scene.directionalLight->BuildDirectionalLightConstants();
    ctx.frame->objectConstantArena.Reset();
    ctx.frame->materialConstantArena.Reset();
    m_bufferFactory.UpdateConstantBuffer(ctx.frame->cameraConstantBuffer, &cameraConstants, sizeof(CameraConstants));
    m_bufferFactory.UpdateConstantBuffer(ctx.frame->directionalLightConstantBuffer, &dirLightConstants, sizeof(Lights::DirectionalLightConstants));
    PIXEndEvent();
}

void D3D12RendererBackend::ProcessUploads()
{
    if (m_uploadContext.queueSize > 0)
    {
        FlushPendingUploads();
    }
    if (m_uploadContext.pendingBatchesSize > 0)
    {
        m_uploadContext.RetireCompletedUploads();
    }
}

void D3D12RendererBackend::EndFrame(const FrameContext& ctx)
{
    ID3D12CommandQueue* queue = m_commandContext.GetDirectCommandQueue();
    m_commandContext.ResourceBarrierTransition(m_swapChain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandContext.EndRecording();
    m_commandContext.ExecuteDirect();
    ctx.frame->fenceValue = m_directFence.Signal(queue);
    m_swapChain.Present();
}

void D3D12RendererBackend::RecordBackBufferSetup(const FrameContext& ctx)
{
    const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_commandContext.SetViewportAndScissor(m_viewport, m_scissorRect);
    m_commandContext.ResourceBarrierTransition(ctx.backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandContext.SetRenderTarget(ctx.backBufferHandle);
    m_commandContext.ClearRenderTarget(ctx.backBufferHandle, clearColor);
}

void D3D12RendererBackend::RecordForwardLitGeometryPass(const FrameContext& ctx, const SceneRenderData& scene)
{
    BindPipeline(ctx.commandList, PipelineId::ForwardLitGeometry);
    ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), m_frameResources[m_swapChain.GetFrameIndex()].cameraConstantBuffer.resource->GetGPUVirtualAddress());
    ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLight), m_frameResources[m_swapChain.GetFrameIndex()].directionalLightConstantBuffer.resource->GetGPUVirtualAddress());
    DrawAllRenderObjects(ctx.commandList, scene);
}

void D3D12RendererBackend::BindPipeline(ID3D12GraphicsCommandList* commandList, PipelineId pipelineId)
{
    const GraphicsPipelineState& pipeline = m_graphicsPipelineLibrary.Get(pipelineId);
    commandList->SetGraphicsRootSignature(pipeline.rootSignature.Get());
    commandList->SetPipelineState(pipeline.pipelineState.Get());
    commandList->IASetPrimitiveTopology(pipeline.primitiveTopology);
}

void D3D12RendererBackend::DrawAllRenderObjects(ID3D12GraphicsCommandList* commandList, const SceneRenderData& scene)
{
    for (const RenderObject& object : *scene.renderObjects)
    {
        ObjectConstants objectConstants = {};
        MaterialConstants materialConstants = {};
        objectConstants.world = efg::Transpose(object.world);
        materialConstants.baseColor = object.material.baseColor;
        materialConstants.specular = object.material.specular;
        D3D12_GPU_VIRTUAL_ADDRESS objectCbAddress = m_bufferFactory.UploadConstantBufferArena(m_frameResources[m_swapChain.GetFrameIndex()].objectConstantArena, &objectConstants, sizeof(ObjectConstants));
        D3D12_GPU_VIRTUAL_ADDRESS materialCbAddress = m_bufferFactory.UploadConstantBufferArena(m_frameResources[m_swapChain.GetFrameIndex()].materialConstantArena, &materialConstants, sizeof(MaterialConstants));
        commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Object), objectCbAddress);
        commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), materialCbAddress);
        DrawMesh(commandList, object.mesh);
    }
}

void D3D12RendererBackend::DrawMesh(ID3D12GraphicsCommandList* commandList, MeshHandle handle)
{
    const GpuMesh& mesh = m_meshLibrary.Get(handle);
    commandList->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);
    if (mesh.indexCount > 0)
    {
        commandList->IASetIndexBuffer(&mesh.indexBufferView);
        commandList->DrawIndexedInstanced(mesh.indexCount, 1, 0, 0, 0);
    }
    else
    {
        commandList->DrawInstanced(mesh.vertexCount, 1, 0, 0);
    }
}