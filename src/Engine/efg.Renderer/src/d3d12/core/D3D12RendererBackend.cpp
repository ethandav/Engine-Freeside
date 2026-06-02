#include "..\..\..\include\render\types\RendererDesc.h"
#include "..\..\..\include\render\types\MeshTypes.h"
#include "..\..\..\include\render\types\Handles.h"
#include "..\..\..\..\efg.Core\include\shapes\skybox.h"

#include "..\..\..\include\d3d12\core\D3D12RendererBackend.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\include\d3d12\core\D3D12Pix.h"
#include "..\..\..\include\d3d12\types\D3D12DrawTypes.h"

#include "..\..\..\include\d3d12\passes\D3D12Pipelines.h"

namespace efg::d3d12
{
    void D3D12RendererBackend::Initialize(const Freeside::RendererDesc& desc)
    {
        InitializeD3D12Systems(desc);
        CreateViewportAndScissor(desc.width, desc.height);
        CreateRenderTargets(desc.width, desc.height);
        CreateFrameResources();
    }

    void D3D12RendererBackend::CreateViewportAndScissor(uint32_t width, uint32_t height)
    {
        m_viewport.TopLeftX = 0.0f;
        m_viewport.TopLeftY = 0.0f;
        m_viewport.Width = static_cast<float>(width);
        m_viewport.Height = static_cast<float>(height);
        m_viewport.MinDepth = 0.0f;
        m_viewport.MaxDepth = 1.0f;

        m_scissorRect.left = 0;
        m_scissorRect.top = 0;
        m_scissorRect.right = static_cast<LONG>(width);
        m_scissorRect.bottom = static_cast<LONG>(height);
    }

    void D3D12RendererBackend::CreateRenderTargets(uint32_t width, uint32_t height)
    {
        m_renderTargets.sceneDepth = m_resources.TextureFactory().CreateDepthBuffer(width, height, DescriptorVisibility::CpuOnly);
    }

    void D3D12RendererBackend::InitializeD3D12Systems(const Freeside::RendererDesc& desc)
    {
        m_device.Initialize(desc);
        m_resources.Initialize(&m_device);

        m_shaderLibrary.Initialize();
        m_shadowSystem.Initialize(&m_resources.TextureFactory());
        m_pipelineFactory.Initialize(m_device.GraphicsContext().GetDevice());
        m_rootSignatureFactory.Initialize(m_device.GraphicsContext().GetDevice());
        m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::ForwardLitGeometry, ForwardLitGeometryPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
        m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::ShadowMap, ShadowMapPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
        m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::Skybox, SkyboxPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
        m_renderServices.buffers = &m_resources.BufferFactory();
        m_renderServices.descriptors = &m_device.DescriptorContext();
        m_renderServices.pipelines = &m_graphicsPipelineLibrary;

        m_renderResources.materials = &m_resources.Materials();
        m_renderResources.meshes = &m_resources.Meshes();
        m_renderResources.textures = &m_resources.MaterialTextures();
    }

    void D3D12RendererBackend::CreateFrameResources()
    {
        for (UINT i = 0; i < NumFramesInFlight; i++)
        {
            m_frameResources[i].commandAllocator = m_device.DirectCommandContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
            m_frameResources[i].uploadBufferArena = m_resources.BufferFactory().CreateUploadBufferArena(100000 * sizeof(InstanceData));
            m_frameResources[i].constantBufferArena = m_resources.BufferFactory().CreateConstantBufferArena(ConstantArenaSize);
            m_frameResources[i].descriptorArena = m_device.DescriptorContext().CreateFrameDescriptorArena(i);
        }
    }

    void D3D12RendererBackend::DestroyFrameResources()
    {
        for (UINT i = 0; i < NumFramesInFlight; i++)
        {
            m_resources.BufferFactory().DestroyUploadBufferArena(m_frameResources[i].uploadBufferArena);
            m_resources.BufferFactory().DestroyConstantBufferArena(m_frameResources[i].constantBufferArena);
        }
    }

    void D3D12RendererBackend::Shutdown()
    {
        m_device.DirectFence().WaitForGPU(m_device.DirectCommandContext().GetDirectCommandQueue());
        DestroyFrameResources();
    }

    void D3D12RendererBackend::Render(const FramePacket& scene)
    {
        FrameContext frameCtx = BeginFrame();

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
        RecordBackBufferSetup(frameCtx);
        PIXEndEvent(commandList);
        PIXBeginEvent(PixColors::ShadowMapPass, L"Skybox Render Pass");
        m_skyboxRenderPass.Execute(passCtx, scene);
        PIXEndEvent();
        PIXBeginEvent(commandList, PixColors::ForwardLitPass, L"Forward Lit Geometry Pass");
        m_forwarLitGeometryRenderPass.Execute(passCtx, scene, shadowMapFrameData);
        PIXEndEvent(commandList);
        PIXEndEvent(commandList);
        m_device.DirectCommandContext().FlushPendingBarrierTransitions();
        EndFrame(frameCtx);
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