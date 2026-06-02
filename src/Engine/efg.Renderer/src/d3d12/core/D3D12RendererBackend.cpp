#include "..\..\..\include\render\types\RendererDesc.h"
#include "..\..\..\include\render\types\MeshTypes.h"
#include "..\..\..\include\render\types\Handles.h"
#include "..\..\..\..\efg.Core\include\shapes\skybox.h"

#include "..\..\..\include\d3d12\core\D3D12RendererBackend.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\include\d3d12\core\D3D12Pix.h"
#include "..\..\..\include\d3d12\core\D3D12Format.h"
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
        CreateBuiltIns();
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
        m_device.Initialize(desc);

        m_resourceFactory.Initialize(m_device.GraphicsContext().GetDevice());
        m_uploadContext.Initialize(&m_device.GraphicsContext(), &m_resourceFactory);
        m_bufferFactory.Initialize(&m_resourceFactory);
        m_textureFactory.Initialize(m_device.GraphicsContext().GetDevice(), &m_resourceFactory, &m_device.DescriptorFactory());
        m_shaderLibrary.Initialize();
        m_shadowSystem.Initialize(&m_textureFactory);
        m_pipelineFactory.Initialize(m_device.GraphicsContext().GetDevice());
        m_rootSignatureFactory.Initialize(m_device.GraphicsContext().GetDevice());
        m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::ForwardLitGeometry, ForwardLitGeometryPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
        m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::ShadowMap, ShadowMapPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
        m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::Skybox, SkyboxPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
        m_renderServices.buffers = &m_bufferFactory;
        m_renderServices.descriptors = &m_device.DescriptorContext();
        m_renderServices.pipelines = &m_graphicsPipelineLibrary;

        m_renderResources.materials = &m_materialLibrary;
        m_renderResources.meshes = &m_meshLibrary;
        m_renderResources.textures = &m_textureLibrary;
    }

    void D3D12RendererBackend::CreateBuiltIns()
    {
        {
            DecodedImage image = m_imageLoader.CreateSolidColorImage(100, 100, 100, 255);
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, ToDxgiFormat(image.format), DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_textureLibrary.RegisterDefaultMaterialTexture2D(texture);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        {
            DecodedImage image = m_imageLoader.CreateSolidColorImage(128, 128, 255, 255);
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, DXGI_FORMAT_R8G8B8A8_UNORM, DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_textureLibrary.RegisterDefaultNormalTexture2D(texture);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        {
            DecodedImage image = m_imageLoader.CreateSolidColorImage(128, 128, 128, 255);
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, DXGI_FORMAT_R8_UNORM, DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_textureLibrary.RegisterDefaultHeightTexture2D(texture);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        {
            std::array<DecodedImage, 6> faces =
            {
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/right.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/left.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/top.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/bottom.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/front.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/back.jpg"),
            };
            /*
            std::array<DecodedImage, 6> faces =
            {
                m_imageLoader.CreateSolidColorImage(255, 0, 0, 255),
                m_imageLoader.CreateSolidColorImage(255, 0, 0, 255),
                m_imageLoader.CreateSolidColorImage(0, 255, 0, 255),
                m_imageLoader.CreateSolidColorImage(0, 255, 0, 255),
                m_imageLoader.CreateSolidColorImage(0, 0, 255, 255),
                m_imageLoader.CreateSolidColorImage(0, 0, 255, 255),
            };
            */
            GpuTextureCube texture = m_textureFactory.CreateTextureCube(faces[0].width, faces[0].height, DescriptorVisibility::CpuOnlyAndShaderVisible, DXGI_FORMAT_R8G8B8A8_UNORM);
            m_textureLibrary.RegisterDefaultSkyboxTexture(texture);
            m_uploadContext.QueueTextureCubeUpload(texture.resource.Get(), faces, texture.resource.Get()->GetDesc(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

            Freeside::MeshData skybox = Freeside::Shapes::SkyboxCube().mesh;
            Freeside::MeshHandle hSkybox = CreateMesh(skybox);

            m_skyboxRenderPass.SetSkyboxMesh(m_meshLibrary.Get(hSkybox));
        }

    }

    void D3D12RendererBackend::CreateFrameResources()
    {
        for (UINT i = 0; i < NumFramesInFlight; i++)
        {
            m_frameResources[i].commandAllocator = m_device.DirectCommandContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
            m_frameResources[i].uploadBufferArena = m_bufferFactory.CreateUploadBufferArena(100000 * sizeof(InstanceData));
            m_frameResources[i].constantBufferArena = m_bufferFactory.CreateConstantBufferArena(ConstantArenaSize);
            m_frameResources[i].descriptorArena = m_device.DescriptorContext().CreateFrameDescriptorArena(i);
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
        ProcessUploads();
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
        Material material = {};
        if (!mat.baseColorTexturePath.empty())
        {
            DecodedImage image = m_imageLoader.LoadImageWithWIC(mat.baseColorTexturePath.c_str());
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, ToDxgiFormat(image.format), DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            material.baseColorTexture = m_textureLibrary.RegisterMaterialTexture2D(texture);
        }

        if (!mat.normalTexturePath.empty())
        {
            DecodedImage image = m_imageLoader.LoadImageWithWIC(mat.normalTexturePath.c_str());
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, DXGI_FORMAT_R8G8B8A8_UNORM, DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            material.normalTexture = m_textureLibrary.RegisterMaterialTexture2D(texture);
        }

        if (!mat.heightTexturePath.empty())
        {
            DecodedImage image = m_imageLoader.LoadHeightMapWithWIC(mat.heightTexturePath.c_str());
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, DXGI_FORMAT_R8_UNORM, DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            material.heightTexture = m_textureLibrary.RegisterMaterialTexture2D(texture);
        }

        MaterialConstants materialConstants{
            Freeside::Math::Vec4(mat.baseColor.x, mat.baseColor.y, mat.baseColor.z, 1.0f),
            Freeside::Math::Vec4(mat.specular.x, mat.specular.y, 0.0f, 0.0f),
            Freeside::Math::Vec4(mat.uvScale.x, mat.uvScale.y, 0.0f, 0.0f)
        };
        material.constants = materialConstants;

        Freeside::MaterialHandle handle = m_materialLibrary.RegisterMaterial(material);
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
        m_uploadContext.copyfence.WaitForQueue(m_device.DirectCommandContext().GetDirectCommandQueue(), ticket.copyFenceValue);
        for (const auto& upload : ticket.resources)
        {
            m_device.DirectCommandContext().ResourceBarrierTransition(upload.destination.Get(), D3D12_RESOURCE_STATE_COPY_DEST, upload.finalState);
        }
    }
}