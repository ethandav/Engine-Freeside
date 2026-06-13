#include "..\..\include\render\Renderer.h"
#include "..\..\include\render\IRendererBackend.h"
#include "..\..\include\d3d12\Core\D3D12RendererBackend.h"
#include "..\..\include\render\RenderThread.h"

namespace Freeside
{
    Renderer::Renderer()
    {
        m_backend = std::make_unique<efg::d3d12::D3D12RendererBackend>();
    }

    Renderer::~Renderer()
    {
        Shutdown();
    }

    void Renderer::Initialize(const Freeside::RendererDesc& desc)
    {
        m_rendererDesc = desc;
        m_backend->Initialize(desc);

        m_renderThread = std::make_unique<efg::RenderThread>();
        m_renderThread->Start(m_backend.get());
    }

    float Renderer::GetRendererAspectRatio()
    {
        return static_cast<float>(m_rendererDesc.width) / static_cast<float>(m_rendererDesc.height);
    }

    void Renderer::Shutdown()
    {
        if (m_renderThread)
        {
            m_renderThread->Stop();
            m_renderThread.reset();
        }

        if (m_backend)
        {
            m_backend->Shutdown();
            m_backend.reset();
        }
    }

    void Renderer::SubmitFrame(efg::FramePacket sceneRenderData)
    {
        m_renderThread->Submit(std::move(sceneRenderData));
    }

    void Renderer::RenderImmediate(efg::FramePacket sceneRenderData)
    {
        m_backend->RenderImmediate(sceneRenderData);
    }

    void Renderer::StartFrame()
    {
        m_backend->StartFrame();
    }

    void Renderer::EndFrame()
    {
        m_backend->EndFrame();
    }

    MeshHandle Renderer::CreateMesh(const MeshDesc& mesh)
    {
        return m_backend->CreateMesh(mesh);
    }

    Freeside::MaterialHandle Renderer::RegisterMaterial(const Freeside::MaterialDesc& mat)
    {
        return m_backend->RegisterMaterial(mat);
    }

    void Renderer::UpdateMaterial(const Freeside::MaterialHandle& handle, const Freeside::MaterialDesc& mat)
    {
        m_backend->UpdateMaterial(handle, mat);
    }

    TextureHandle Renderer::CreateMaterialTexture2d(const TextureDesc& texture)
    {
        return m_backend->CreateMaterialTexture2d(texture);
    }

    TextureHandle Renderer::CreateTextureCube(const std::array<TextureDesc, 6> faces)
    {
        return m_backend->CreateTextureCube(faces);
    }

#if defined(EFG_ENABLE_IMGUI)
    bool Renderer::HandleNativeWindowMessage(void* hwnd, uint32_t message, uintptr_t wParam, intptr_t lParam, intptr_t& outResult)
    {
        return m_backend->HandleImguiWindowMessage(
            hwnd,
            message,
            wParam,
            lParam,
            outResult
        );
    }
#endif

#if defined(FREESIDE_EDITOR)
    uint64_t Renderer::GetTextureID(TextureHandle handle)
    {
        return m_backend->GetTextureID(handle);
    }
#endif
}