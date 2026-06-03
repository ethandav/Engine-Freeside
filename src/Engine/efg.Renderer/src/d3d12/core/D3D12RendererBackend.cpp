#include "..\..\..\include\d3d12\core\D3D12RendererBackend.h"
#include "..\..\..\include\d3d12\core\D3D12Pix.h"


namespace efg::d3d12
{
    void D3D12RendererBackend::Initialize(const Freeside::RendererDesc& desc)
    {
        InitializeD3D12Systems(desc);
        CreateRenderTargets(desc.width, desc.height);
    }

    void D3D12RendererBackend::CreateRenderTargets(uint32_t width, uint32_t height)
    {
        m_renderTargets.sceneDepth = m_resources.TextureFactory().CreateDepthBuffer(width, height, DescriptorVisibility::CpuOnly);
    }

    void D3D12RendererBackend::InitializeD3D12Systems(const Freeside::RendererDesc& desc)
    {
        m_device.Initialize(desc);
        m_resources.Initialize(&m_device);
        m_pipeline.Initialize(m_device.GraphicsContext().GetDevice());
        m_frame.Initialize(&m_device, &m_resources, desc.width, desc.height);
        m_passes.Initialize(&m_device, &m_resources, &m_pipeline, &m_frame, &m_renderTargets);
    }

    void D3D12RendererBackend::Shutdown()
    {
        m_device.DirectFence().WaitForGPU(m_device.DirectCommandContext().GetDirectCommandQueue());
        m_frame.Shutdown();
    }

    void D3D12RendererBackend::Render(const FramePacket& scene)
    {
        FrameContext frameCtx = m_frame.BeginFrame(&m_renderQueue);
        m_passes.Execute(scene, frameCtx, m_renderQueue);
        m_frame.EndFrame(frameCtx);
    }

    Freeside::MeshHandle D3D12RendererBackend::CreateMesh(const Freeside::MeshData& mesh)
    {
        return m_resources.CreateMesh(mesh);
    }

    Freeside::MaterialHandle D3D12RendererBackend::RegisterMaterial(const Freeside::MaterialDesc& mat)
    {
        return m_resources.RegisterMaterial(mat);
    }
}