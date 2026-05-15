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

    MeshHandle Renderer::CreateMesh(const MeshData& mesh)
    {
        return m_backend->CreateMesh(mesh);
    }

    Freeside::MaterialHandle Renderer::RegisterMaterial(const Freeside::MaterialDesc& mat)
    {
        return m_backend->RegisterMaterial(mat);
    }
    TextureHandle Renderer::RegisterTexture2D(const wchar_t* filename)
    {
        return m_backend->RegisterTexture2D(filename);
    }
}