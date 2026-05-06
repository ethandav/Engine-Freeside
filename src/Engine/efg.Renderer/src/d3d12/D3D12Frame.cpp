#include "..\..\include\d3d12\D3D12RendererBackend.h"
#include "../../include/Camera.h"
#include "..\..\include\ShaderConstants.h"

void D3D12RendererBackend::BeginFrame(efg::Camera* camera)
{
    UINT frameIndex = m_swapChain.GetFrameIndex();
    FrameResource& frame = m_frameResources[frameIndex];
    ID3D12CommandAllocator* allocator = frame.commandAllocator.Get();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain.GetCurrentRTV();
    ID3D12GraphicsCommandList* commandList = m_commandContext.GetDirectCommandList();
    const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_directFence.WaitForCPU(frame.fenceValue);
    frame.objectConstantArena.Reset();
    m_commandContext.BeginRecording(allocator);
    if (m_uploadContext.queueSize > 0)
    {
        FlushPendingUploads();
    }
    if (m_uploadContext.pendingBatchesSize > 0)
    {
        m_uploadContext.RetireCompletedUploads();
    }

    CameraConstants cameraConstants = {};
    DirectionalLightConstants dirLightConstants = {};
    cameraConstants.viewProjection = efg::Transpose(camera->GetViewProjectionMatrix());
    dirLightConstants.directionAndIntensity = efg::Vec4(-0.2f, -1.0f, -0.3f, 1.0f);
    dirLightConstants.colorAndPadding = efg::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    m_bufferFactory.UpdateConstantBuffer(frame.cameraConstantBuffer, &cameraConstants, sizeof(CameraConstants));
    m_bufferFactory.UpdateConstantBuffer(frame.directionalLightConstantBuffer, &dirLightConstants, sizeof(DirectionalLightConstants));

    m_commandContext.SetViewportAndScissor(m_viewport, m_scissorRect);
    m_commandContext.ResourceBarrierTransition(m_swapChain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandContext.SetRenderTarget(rtvHandle);
    m_commandContext.ClearRenderTarget(rtvHandle, clearColor);
    DrawAllRenderObjects(commandList);
}

void D3D12RendererBackend::EndFrame()
{
    ID3D12CommandQueue* queue = m_commandContext.GetDirectCommandQueue();
    FrameResource& frame = m_frameResources[m_swapChain.GetFrameIndex()];
    m_commandContext.ResourceBarrierTransition(m_swapChain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandContext.EndRecording();
    m_commandContext.ExecuteDirect();
    frame.fenceValue = m_directFence.Signal(queue);
    m_swapChain.Present();
}

void D3D12RendererBackend::DrawAllRenderObjects(ID3D12GraphicsCommandList* commandList)
{
    const GraphicsPipelineState& pipeline = m_graphicsPipelineLibrary.Get(PipelineId::ForwardLitGeometry);

    commandList->SetGraphicsRootSignature(pipeline.rootSignature.Get());
    commandList->SetPipelineState(pipeline.pipelineState.Get());
    commandList->IASetPrimitiveTopology(pipeline.primitiveTopology);
    commandList->SetGraphicsRootConstantBufferView(0, m_frameResources[m_swapChain.GetFrameIndex()].cameraConstantBuffer.resource->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(2, m_frameResources[m_swapChain.GetFrameIndex()].directionalLightConstantBuffer.resource->GetGPUVirtualAddress());

    for (const auto& object : m_renderObjects)
    {
        ObjectConstants objectConstants = {};
        objectConstants.world = efg::Transpose(object->world);
        D3D12_GPU_VIRTUAL_ADDRESS objectCbAddress = m_bufferFactory.UploadConstantBufferArena(m_frameResources[m_swapChain.GetFrameIndex()].objectConstantArena, &objectConstants, sizeof(ObjectConstants));
        commandList->SetGraphicsRootConstantBufferView(1, objectCbAddress);
        DrawMesh(commandList, object->mesh);
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