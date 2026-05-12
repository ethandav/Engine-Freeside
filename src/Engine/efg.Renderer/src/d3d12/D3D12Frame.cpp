#include "..\..\include\d3d12\D3D12RendererBackend.h"
#include "..\..\include\render\Camera.h"
#include "..\..\include\render\RenderTypes.h"
#include "..\..\include\d3d12\D3D12Pix.h"
#include <algorithm>

namespace efg::d3d12
{
    FrameContext D3D12RendererBackend::BeginFrame()
    {
        UINT frameIndex = m_swapChain.GetFrameIndex();
        FrameResource& frame = m_frameResources[frameIndex];
        ID3D12CommandAllocator* allocator = frame.commandAllocator.Get();

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
        m_commandContext.SetRenderTarget(ctx.backBufferHandle, ctx.frame->depthBuffer.dsv);
        m_commandContext.ClearRenderTarget(ctx.backBufferHandle, clearColor);
        m_commandContext.ClearDepthStencil(ctx.frame->depthBuffer.dsv, 1.0f, 0);
    }
}