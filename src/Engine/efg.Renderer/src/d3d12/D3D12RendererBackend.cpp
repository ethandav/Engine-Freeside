#include "..\..\include\d3d12\D3D12RendererBackend.h"
#include "..\..\include\render\Camera.h"
#include "..\..\include\render\RenderTypes.h"
#include "..\..\include\d3d12\D3D12Pix.h"

#include <wincodec.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <stdexcept>

#pragma comment(lib, "windowscodecs.lib")

namespace efg::d3d12
{

    DecodedImage LoadImageWithWIC(const wchar_t* filePath)
    {
        using Microsoft::WRL::ComPtr;

        ComPtr<IWICImagingFactory> factory;
        D3D12_THROW_IF_FAILED(CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(factory.GetAddressOf())
        ));

        ComPtr<IWICBitmapDecoder> decoder;
        D3D12_THROW_IF_FAILED(factory->CreateDecoderFromFilename(
            filePath,
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            decoder.GetAddressOf()
        ));

        ComPtr<IWICBitmapFrameDecode> frame;
        D3D12_THROW_IF_FAILED(decoder->GetFrame(0, frame.GetAddressOf()));

        UINT width = 0;
        UINT height = 0;
        D3D12_THROW_IF_FAILED(frame->GetSize(&width, &height));

        ComPtr<IWICFormatConverter> converter;
        D3D12_THROW_IF_FAILED(factory->CreateFormatConverter(converter.GetAddressOf()));

        D3D12_THROW_IF_FAILED(converter->Initialize(
            frame.Get(),
            GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom
        ));

        DecodedImage image = {};
        image.width = width;
        image.height = height;
        image.rowPitch = width * 4;
        image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
        image.pixels.resize(static_cast<size_t>(image.rowPitch) * height);

        D3D12_THROW_IF_FAILED(converter->CopyPixels(
            nullptr,
            image.rowPitch,
            static_cast<UINT>(image.pixels.size()),
            image.pixels.data()
        ));

        return image;
    }

    void D3D12RendererBackend::Initialize(const RendererDesc& desc)
    {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        CreateViewportAndScissor(desc.width, desc.height);
        InitializeD3D12Systems(desc);
        InitializeRenderPasses();
        CreateFrameResources(desc.width, desc.height);
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

    void D3D12RendererBackend::InitializeD3D12Systems(const RendererDesc& desc)
    {
        m_graphicsContext.Initialize(false);
        m_commandContext.Initialize(&m_graphicsContext);
        m_uploadContext.Initialize(&m_graphicsContext);
        m_swapChain.Initialize(&m_graphicsContext, &m_commandContext, &m_descriptorContext);
        m_descriptorContext.Initialize(m_graphicsContext.GetDevice());
        m_resourceFactory.Initialize(m_graphicsContext.GetDevice());
        m_bufferFactory.Initialize(&m_resourceFactory);
        m_directFence.Initialize(&m_graphicsContext);
        m_swapChain.CreateSwapChain(desc.nativeWindowHandle, desc.width, desc.height);
        m_descriptorContext.CreateAllHeaps();
        m_swapChain.CreateBackBufferViews();
        m_directFence.CreateFence(0);
        m_shaderLibrary.Initialize();
        m_graphicsPipelineLibrary.Initialize(&m_graphicsContext, m_shaderLibrary);
    }

    void D3D12RendererBackend::InitializeRenderPasses()
    {
        m_forwarLitGeometryRenderPass.Initialize(&m_graphicsPipelineLibrary, &m_descriptorContext, &m_meshLibrary, &m_materialLibrary, &m_textureLibrary, &m_bufferFactory);
    }

    void D3D12RendererBackend::CreateFrameResources(uint32_t width, uint32_t height)
    {
        for (UINT i = 0; i < NumFramesInFlight; i++)
        {
            m_frameResources[i].commandAllocator = m_commandContext.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
            m_frameResources[i].uploadBufferArena = m_bufferFactory.CreateUploadBufferArena(100000 * sizeof(InstanceData));
            m_frameResources[i].constantBufferArena = m_bufferFactory.CreateConstantBufferArena(ConstantArenaSize);
            m_frameResources[i].depthBuffer = m_bufferFactory.CreateDepthBuffer(width, height);
            m_frameResources[i].depthBuffer.dsv = m_descriptorContext.CreateDSV(m_frameResources[i].depthBuffer.resource.Get(), nullptr).cpu;
        }
    }

    void D3D12RendererBackend::DestroyFrameResources()
    {

    }

    void D3D12RendererBackend::Shutdown()
    {
        m_directFence.WaitForGPU(m_commandContext.GetDirectCommandQueue());
        DestroyFrameResources();
    }

    void D3D12RendererBackend::Render(const FramePacket& scene)
    {
        FrameContext ctx = BeginFrame();
        PIXBeginEvent(ctx.commandList, PIX_COLOR(100, 100, 255), L"BeginFrame");
        ProcessUploads();
        PIXBeginEvent(ctx.commandList, PIX_COLOR(100, 100, 255), L"BackBufferSetup");
        RecordBackBufferSetup(ctx);
        PIXEndEvent(ctx.commandList); // BackBufferSetup End
        m_renderQueue.BuildForwardGeometryBatches(scene.renderObjects);
        PIXBeginEvent(ctx.commandList, PIX_COLOR(100, 100, 255), L"ForwardLitGeometryPass");
        m_forwarLitGeometryRenderPass.Execute(ctx, scene);
        PIXEndEvent(ctx.commandList); // ForwardLitGeometryPass End
        PIXEndEvent(ctx.commandList); // BeginFrame End
        EndFrame(ctx);
    }

    MeshHandle D3D12RendererBackend::CreateMesh(const MeshData& mesh)
    {
        MeshHandle handle = m_meshLibrary.RegisterMesh(mesh);
        GpuBuffer vertexBuffer = m_bufferFactory.CreateStaticBuffer(mesh.vertices.data(), (mesh.vertices.size() * sizeof(Vertex)));
        GpuBuffer indexBuffer = m_bufferFactory.CreateStaticBuffer(mesh.indices.data(), (mesh.indices.size() * sizeof(uint32_t)));
        m_meshLibrary.SetVertexBuffer(handle, vertexBuffer);
        m_meshLibrary.SetIndexBuffer(handle, indexBuffer);
        m_uploadContext.QueueBufferForUpload(vertexBuffer.resource.Get(), vertexBuffer.uploadResource.Get(), vertexBuffer.sizeInBytes, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        m_uploadContext.QueueBufferForUpload(indexBuffer.resource.Get(), indexBuffer.uploadResource.Get(), indexBuffer.sizeInBytes, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        return handle;
    }

    efg::MaterialHandle D3D12RendererBackend::RegisterMaterial(const efg::MaterialDesc& mat)
    {
        MaterialHandle handle = m_materialLibrary.RegisterMaterial(mat);
        return handle;
    }

    TextureHandle D3D12RendererBackend::RegisterTexture2D(const wchar_t* filename)
    {
        DecodedImage image = LoadImageWithWIC(filename);

        GpuTexture2D texture =
            m_resourceFactory.CreateTexture2DFromMemory(
                image.pixels.data(),
                image.width,
                image.height,
                image.format,
                image.rowPitch
            );

        m_uploadContext.QueueTextureForUpload(texture.resource.Get(), texture.uploadResource.Get(), texture.uploadFootprint, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        DescriptorAllocation alloc = m_descriptorContext.CreateTexture2DSRV(texture.resource.Get(), texture.format, texture.mipLevels);
        texture.gpuSrv = alloc.gpu;
        texture.cpuSrv = alloc.cpu;
        TextureHandle handle = m_textureLibrary.RegisterTexture2D(texture);

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