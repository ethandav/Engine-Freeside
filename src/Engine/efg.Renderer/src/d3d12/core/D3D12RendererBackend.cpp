#include "..\..\..\include\render\types\RendererDesc.h"
#include "..\..\..\include\render\types\MeshTypes.h"
#include "..\..\..\include\render\types\Handles.h"
#include "..\..\..\..\efg.Core\include\shapes\skybox.h"

#include "..\..\..\include\d3d12\core\D3D12RendererBackend.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
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
        
        m_shadowSystem.Initialize(&m_resources.TextureFactory());

        m_renderServices.buffers = &m_resources.BufferFactory();
        m_renderServices.descriptors = &m_device.DescriptorContext();
        m_renderServices.pipelines = &m_pipeline.GraphicsPipelineLibrary();

        m_renderResources.materials = &m_resources.Materials();
        m_renderResources.meshes = &m_resources.Meshes();
        m_renderResources.textures = &m_resources.MaterialTextures();
    }

    void D3D12RendererBackend::Shutdown()
    {
        m_device.DirectFence().WaitForGPU(m_device.DirectCommandContext().GetDirectCommandQueue());
        m_frame.Shutdown();
    }

    void D3D12RendererBackend::Render(const FramePacket& scene)
    {
        FrameContext frameCtx = m_frame.BeginFrame(&m_renderQueue);

        D3D12PassContext passCtx = {};
        passCtx.frameContext = &frameCtx;
        passCtx.services = &m_renderServices;
        passCtx.libraries = &m_renderResources;
        passCtx.renderQueue = &m_renderQueue;

        ID3D12GraphicsCommandList* commandList = frameCtx.commandContext->GetDirectCommandList();
        PIXBeginEvent(commandList, PIX_COLOR(0, 0, 0), L"Begin Frame");
        m_resources.ProcessUploads(&m_device.DirectCommandContext());
        m_renderQueue.BuildForwardGeometryBatches(scene.renderObjects);
        PIXBeginEvent(PixColors::ShadowMapPass, L"Shadow System Update");
        ShadowMapFrameData shadowMapFrameData = m_shadowSystem.Update(scene);
        PIXEndEvent();
        PIXBeginEvent(commandList, PixColors::ShadowMapPass, L"Shadow Map Pass");
        m_shadowMapRenderPass.Execute(passCtx, scene, shadowMapFrameData);
        PIXEndEvent(commandList);
        m_device.DirectCommandContext().FlushPendingBarrierTransitions();
        PIXBeginEvent(commandList, PixColors::BackbufferSetup, L"BackBuffer Setup");
        m_frame.RecordBackBufferSetup(frameCtx, m_renderTargets);
        PIXEndEvent(commandList);
        PIXBeginEvent(PixColors::ShadowMapPass, L"Skybox Render Pass");
        m_skyboxRenderPass.Execute(passCtx, scene);
        PIXEndEvent();
        PIXBeginEvent(commandList, PixColors::ForwardLitPass, L"Forward Lit Geometry Pass");
        m_forwarLitGeometryRenderPass.Execute(passCtx, scene, shadowMapFrameData);
        PIXEndEvent(commandList);
        PIXEndEvent(commandList);
        m_device.DirectCommandContext().FlushPendingBarrierTransitions();
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