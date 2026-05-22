#include "..\..\..\include\render\types\RendererDesc.h"
#include "..\..\..\include\render\types\MeshTypes.h"
#include "..\..\..\include\render\types\Handles.h"

#include "..\..\..\include\d3d12\core\D3D12RendererBackend.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\include\d3d12\core\D3D12Pix.h"
#include "..\..\..\include\d3d12\core\D3D12Format.h"
#include "..\..\..\include\d3d12\types\D3D12DrawTypes.h"

namespace efg::d3d12
{
    void D3D12RendererBackend::Initialize(const Freeside::RendererDesc& desc)
    {
        InitializeD3D12Systems(desc);
        CreateViewportAndScissor(desc.width, desc.height);
        CreateRenderTargets(desc.width, desc.height);
        InitializeRenderPasses();
        CreateFrameResources();
        m_directFence.WaitForGPU(m_commandContext.GetDirectCommandQueue());
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
        m_renderTargets.sceneDepth = m_textureFactory.CreateDepthBuffer(width, height, DescriptorVisibility::CpuOnly);
    }

    void D3D12RendererBackend::InitializeD3D12Systems(const Freeside::RendererDesc& desc)
    {
        m_graphicsContext.Initialize(false);
        m_commandContext.Initialize(&m_graphicsContext);
        m_descriptorContext.Initialize(m_graphicsContext.GetDevice());
        m_swapChain.Initialize(&m_graphicsContext, &m_commandContext, &m_descriptorFactory);
        m_resourceFactory.Initialize(m_graphicsContext.GetDevice());
        m_uploadContext.Initialize(&m_graphicsContext, &m_resourceFactory);
        m_bufferFactory.Initialize(&m_resourceFactory);
        m_textureFactory.Initialize(m_graphicsContext.GetDevice(), &m_resourceFactory, &m_descriptorFactory);
        m_descriptorFactory.Initialize(m_graphicsContext.GetDevice(), &m_descriptorContext);
        m_directFence.Initialize(&m_graphicsContext);
        m_swapChain.CreateSwapChain(desc.nativeWindowHandle, desc.width, desc.height);
        m_descriptorContext.CreateAllHeaps();
        m_swapChain.CreateBackBufferViews();
        m_directFence.CreateFence(0);
        m_shaderLibrary.Initialize();
        m_graphicsPipelineLibrary.Initialize(&m_graphicsContext, m_shaderLibrary);
        m_shadowSystem.Initialize(&m_textureFactory);
    }

    void D3D12RendererBackend::InitializeRenderPasses()
    {
        m_forwarLitGeometryRenderPass.Initialize(&m_graphicsPipelineLibrary, &m_descriptorContext, &m_meshLibrary, &m_materialLibrary, &m_textureLibrary, &m_bufferFactory);
        m_shadowMapRenderPass.Initialize(&m_graphicsPipelineLibrary, &m_descriptorContext, &m_meshLibrary, &m_textureFactory, &m_bufferFactory);
    }

    void D3D12RendererBackend::CreateFrameResources()
    {
        for (UINT i = 0; i < NumFramesInFlight; i++)
        {
            m_frameResources[i].commandAllocator = m_commandContext.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
            m_frameResources[i].uploadBufferArena = m_bufferFactory.CreateUploadBufferArena(100000 * sizeof(InstanceData));
            m_frameResources[i].constantBufferArena = m_bufferFactory.CreateConstantBufferArena(ConstantArenaSize);
            m_frameResources[i].descriptorArena = m_descriptorContext.CreateDescriptorArena(i);
        }
    }

    void D3D12RendererBackend::DestroyFrameResources()
    {
        for (UINT i = 0; i < NumFramesInFlight; i++)
        {
            m_bufferFactory.DestroyUploadBufferArena(m_frameResources[i].uploadBufferArena);
            m_bufferFactory.DestroyConstantBufferArena(m_frameResources[i].constantBufferArena);
        }
    }

    void D3D12RendererBackend::Shutdown()
    {
        m_directFence.WaitForGPU(m_commandContext.GetDirectCommandQueue());
        DestroyFrameResources();
    }

    void D3D12RendererBackend::Render(const FramePacket& scene)
    {
        FrameContext ctx = BeginFrame();
        ID3D12GraphicsCommandList* commandList = ctx.commandContext->GetDirectCommandList();

        PIXBeginEvent(commandList, PIX_COLOR(0, 0, 0), L"Begin Frame");
        ProcessUploads();
        PIXBeginEvent(PixColors::ShadowMapPass, L"");
        m_renderQueue.BuildForwardGeometryBatches(scene.renderObjects);
        PIXEndEvent();
        PIXBeginEvent(PixColors::ShadowMapPass, L"Shadow System Update");
        ShadowMapFrameData shadowMapFrameData = m_shadowSystem.Update(scene);
        PIXEndEvent();
        PIXBeginEvent(commandList, PixColors::ShadowMapPass, L"Shadow Map Pass");
        m_shadowMapRenderPass.Execute(ctx, scene, shadowMapFrameData);
        PIXEndEvent(commandList);
        m_commandContext.FlushPendingBarrierTransitions();
        PIXBeginEvent(commandList, PixColors::BackbufferSetup, L"BackBuffer Setup");
        RecordBackBufferSetup(ctx);
        PIXEndEvent(commandList);
        PIXBeginEvent(commandList, PixColors::ForwardLitPass, L"Forward Lit Geometry Pass");
        m_forwarLitGeometryRenderPass.Execute(ctx, scene, shadowMapFrameData);
        PIXEndEvent(commandList);
        PIXEndEvent(commandList);
        m_commandContext.FlushPendingBarrierTransitions();
        EndFrame(ctx);
    }

    Freeside::MeshHandle D3D12RendererBackend::CreateMesh(const Freeside::MeshData& mesh)
    {
        Freeside::MeshHandle handle = m_meshLibrary.RegisterMesh(mesh);
        GpuBuffer vertexBuffer = m_bufferFactory.CreateStaticBuffer((mesh.vertices.size() * sizeof(Freeside::Vertex)));
        GpuBuffer indexBuffer = m_bufferFactory.CreateStaticBuffer((mesh.indices.size() * sizeof(uint32_t)));
        m_meshLibrary.SetVertexBuffer(handle, vertexBuffer);
        m_meshLibrary.SetIndexBuffer(handle, indexBuffer);
        m_uploadContext.QueueBufferUpload(vertexBuffer.resource.Get(), mesh.vertices.data(), vertexBuffer.sizeInBytes, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        m_uploadContext.QueueBufferUpload(indexBuffer.resource.Get(), mesh.indices.data(), indexBuffer.sizeInBytes, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        return handle;
    }

    Freeside::MaterialHandle D3D12RendererBackend::RegisterMaterial(const Freeside::MaterialDesc& mat)
    {
        Freeside::MaterialHandle handle = m_materialLibrary.RegisterMaterial(mat);
        return handle;
    }

    Freeside::TextureHandle D3D12RendererBackend::RegisterTexture2D(const wchar_t* filename)
    {
        DecodedImage image = m_imageLoader.LoadImageWithWIC(filename);
        GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, ToDxgiFormat(image.format), DescriptorVisibility::ShaderVisible);
        m_uploadContext.QueueTextureUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        Freeside::TextureHandle handle = m_textureLibrary.RegisterTexture2D(texture);

        return handle;
    }

    void D3D12RendererBackend::ProcessUploads()
    {
        if (m_uploadContext.queueSize > 0)
        {
            UploadTicket uploadTicket = m_uploadContext.FlushUploads();
            RecordUploadedResourceTransitions(uploadTicket);
        }
        if (m_uploadContext.pendingBatchesSize > 0)
        {
            m_uploadContext.RetireCompletedUploads();
        }
    }

    void D3D12RendererBackend::RecordUploadedResourceTransitions(const UploadTicket& ticket)
    {
        m_uploadContext.copyfence.WaitForQueue(m_commandContext.GetDirectCommandQueue(), ticket.copyFenceValue);
        for (const auto& upload : ticket.resources)
        {
            m_commandContext.ResourceBarrierTransition(upload.destination.Get(), D3D12_RESOURCE_STATE_COPY_DEST, upload.finalState);
        }
    }
}