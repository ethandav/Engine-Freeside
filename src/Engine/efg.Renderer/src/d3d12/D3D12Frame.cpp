#include "..\..\include\d3d12\D3D12RendererBackend.h"
#include "..\..\include\Camera.h"
#include "..\..\include\ShaderConstants.h"
#include "..\..\include\d3d12\D3D12Pix.h"
#include <algorithm>

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

void D3D12RendererBackend::UpdateFrameConstants(const FrameContext& ctx, const FramePacket& scene)
{
    PIXBeginEvent(PIX_COLOR(255, 255, 255), L"Update Frame Constants");
    CameraConstants cameraConstants = scene.camera.BuildCameraConstants();
    Lights::DirectionalLightConstants dirLightConstants = scene.directionalLight.BuildDirectionalLightConstants();
    ctx.frame->objectConstantArena.Reset();
    ctx.frame->materialConstantArena.Reset();
    ctx.frame->gpuUploadBufferArena.Reset();
    m_bufferFactory.UpdateConstantBuffer(ctx.frame->cameraConstantBuffer, &cameraConstants, sizeof(CameraConstants));
    m_bufferFactory.UpdateConstantBuffer(ctx.frame->directionalLightConstantBuffer, &dirLightConstants, sizeof(Lights::DirectionalLightConstants));
    PIXEndEvent();
}

void D3D12RendererBackend::UpdatePointLights(const FrameContext& ctx, const FramePacket& scene)
{
    Lights::PointLightConstants metadata = {};
    uint32_t count = 0;

    if (!scene.pointLights.empty())
    {
        count = static_cast<uint32_t>(std::min<size_t>(scene.pointLights.size(), ctx.frame->pointLightStructuredBuffer.elementCount));
        Lights::GpuPointLight* dst = reinterpret_cast<Lights::GpuPointLight*>(ctx.frame->pointLightStructuredBuffer.mappedData);

        for (uint32_t i = 0; i < count; ++i)
        {
            const Lights::Point& light = (scene.pointLights)[i];

            dst[i].positionAndRadius = {
                light.position.x,
                light.position.y,
                light.position.z,
                light.radius
            };

            dst[i].colorAndIntensity = {
                light.color.x,
                light.color.y,
                light.color.z,
                light.intensity
            };
        }
    }

    metadata.pointLightCount = count;
    m_bufferFactory.UpdateConstantBuffer(ctx.frame->pointLightConstantBuffer, &metadata, sizeof(Lights::PointLightConstants));
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
    m_commandContext.SetRenderTarget(ctx.backBufferHandle, ctx.frame->depthBuffer.cpuSrv);
    m_commandContext.ClearRenderTarget(ctx.backBufferHandle, clearColor);
    m_commandContext.ClearDepthStencil(ctx.frame->depthBuffer.cpuSrv, 1.0f, 0);
}

void D3D12RendererBackend::RecordForwardLitGeometryPass(const FrameContext& ctx, const FramePacket& scene)
{
    BindPipeline(ctx.commandList, PipelineId::ForwardLitGeometry);
    ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), m_frameResources[m_swapChain.GetFrameIndex()].cameraConstantBuffer.resource->GetGPUVirtualAddress());
    ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLight), m_frameResources[m_swapChain.GetFrameIndex()].directionalLightConstantBuffer.resource->GetGPUVirtualAddress());
    ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::PointLightConstants), m_frameResources[m_swapChain.GetFrameIndex()].pointLightConstantBuffer.resource->GetGPUVirtualAddress());

    ID3D12DescriptorHeap* heaps[] = { m_descriptorContext.GetCBVSRVUAVHeap() };
    ctx.commandList->SetDescriptorHeaps(_countof(heaps), heaps);
    ctx.commandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(ForwardLitRootParameter::PointLightsSrv), ctx.frame->pointLightStructuredBuffer.gpuSrv);
    DrawAllRenderObjects(ctx, scene);
}

void D3D12RendererBackend::BindPipeline(ID3D12GraphicsCommandList* commandList, PipelineId pipelineId)
{
    const GraphicsPipelineState& pipeline = m_graphicsPipelineLibrary.Get(pipelineId);
    commandList->SetGraphicsRootSignature(pipeline.rootSignature.Get());
    commandList->SetPipelineState(pipeline.pipelineState.Get());
    commandList->IASetPrimitiveTopology(pipeline.primitiveTopology);
}

void D3D12RendererBackend::DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene)
{
    std::vector<uint32_t> sortedIndices(scene.renderObjects.size());

    SortRenderObjectsForInstancing(sortedIndices, scene);
    uint32_t begin = 0;

    while (begin < sortedIndices.size())
    {
        const RenderObject& first = scene.renderObjects[sortedIndices[begin]];

        uint32_t end = begin + 1;

        while (end < sortedIndices.size())
        {
            const RenderObject& candidate = scene.renderObjects[sortedIndices[end]];

            if (candidate.mesh != first.mesh ||
                candidate.material != first.material)
            {
                break;
            }

            ++end;
        }

        // Objects in [begin, end) are one batch.
        DrawInstancedBatch(ctx, scene, sortedIndices, begin, end);

        begin = end;
    }
}

void D3D12RendererBackend::SortRenderObjectsForInstancing(std::vector<uint32_t>& sortedIndices, const FramePacket& scene)
{
    for (uint32_t i = 0; i < sortedIndices.size(); ++i)
    {
        sortedIndices[i] = i;
    }

    std::sort(sortedIndices.begin(), sortedIndices.end(),
        [&](uint32_t lhs, uint32_t rhs)
        {
            const RenderObject& a = scene.renderObjects[lhs];
            const RenderObject& b = scene.renderObjects[rhs];

            if (a.material.index != b.material.index)
                return a.material.index < b.material.index;

            return a.mesh.index < b.mesh.index;
        });
}

void D3D12RendererBackend::DrawInstancedBatch(const FrameContext& ctx, const FramePacket& scene, const std::vector<uint32_t>& sortedIndices, uint32_t begin, uint32_t end)
{
    const RenderObject& first = scene.renderObjects[sortedIndices[begin]];
    const GpuMaterial& material = first.material.IsValid() ? m_materialLibrary.GetMaterialByHandle(first.material) : m_materialLibrary.GetDefaultMaterial();
    D3D12_GPU_VIRTUAL_ADDRESS materialCbAddress = m_bufferFactory.UploadConstantBufferArena(ctx.frame->materialConstantArena, &material, sizeof(GpuMaterial));
    ctx.commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), materialCbAddress);
    std::vector<InstanceData> instances;
    instances.reserve(end - begin);

    for (uint32_t i = begin; i < end; ++i)
    {
        const RenderObject& object = scene.renderObjects[sortedIndices[i]];
        InstanceData instance = {};
        instance.world = efg::Transpose(object.world);
        instances.push_back(instance);
    }

    D3D12_GPU_VIRTUAL_ADDRESS instanceBufferAddress = m_bufferFactory.UploadBufferArena(ctx.frame->gpuUploadBufferArena, instances.data(), instances.size() * sizeof(InstanceData));
    ctx.commandList->SetGraphicsRootShaderResourceView(static_cast<UINT>(ForwardLitRootParameter::InstanceData), instanceBufferAddress);
    DrawMeshInstanced(ctx.commandList, first.mesh, static_cast<uint32_t>(instances.size()));
}

void D3D12RendererBackend::DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, efg::MeshHandle handle, uint32_t instanceCount)
{
    const GpuMesh& mesh = m_meshLibrary.Get(handle);
    commandList->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);
    if (mesh.indexCount > 0)
    {
        commandList->IASetIndexBuffer(&mesh.indexBufferView);
        commandList->DrawIndexedInstanced(mesh.indexCount, instanceCount, 0, 0, 0);

    }
    else
    {
        commandList->DrawInstanced(mesh.vertexCount, instanceCount, 0, 0);
    }
}
