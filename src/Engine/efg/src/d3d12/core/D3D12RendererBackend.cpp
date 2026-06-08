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

        GpuTexture2D sceneColor = {};
        sceneColor.resourceFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        sceneColor.rtvFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        sceneColor.flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        sceneColor.srvFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        sceneColor.initialState = D3D12_RESOURCE_STATE_PRESENT;
        sceneColor.width = width;
        sceneColor.height = height;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        clearValue.Color[0] = 0.0f;
        clearValue.Color[1] = 0.0f;
        clearValue.Color[2] = 0.0f;
        clearValue.Color[3] = 1.0f;

        m_resources.TextureFactory().CreateTexture2D(sceneColor, DescriptorVisibility::CpuOnlyAndShaderVisible, &clearValue);
        m_renderTargets.sceneColor = sceneColor;

        sceneColor.resource.Get()->SetName(L"HDR Scene Color Render Target");
        m_renderTargets.sceneDepth.resource.Get()->SetName(L"Scene Depth Buffer");
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
        m_frame.EndFrame(frameCtx, m_renderTargets);
    }

    Freeside::MeshHandle D3D12RendererBackend::CreateMesh(const Freeside::MeshDesc& mesh)
    {
        return m_resources.CreateMesh(mesh);
    }

    Freeside::MaterialHandle D3D12RendererBackend::RegisterMaterial(const Freeside::MaterialDesc& mat)
    {
        return m_resources.RegisterMaterial(mat);
    }

    Freeside::TextureHandle D3D12RendererBackend::CreateMaterialTexture2d(const Freeside::TextureDesc& texture)
    {
        return m_resources.CreateMaterialTexture2d(texture);
    }

    Freeside::TextureHandle D3D12RendererBackend::CreateTextureCube(const std::array<Freeside::TextureDesc, 6> faces)
    {
        return m_resources.CreateTextureCube(faces);
    }
}