#include "..\include\Renderer.h"
#include "..\include\IRendererBackend.h"
#include "..\include\d3d12\D3D12RendererBackend.h"
#include "..\include\RenderThread.h"

Renderer::Renderer()
{
    m_backend = std::make_unique<D3D12RendererBackend>();
}

Renderer::~Renderer()
{
    Shutdown();
}

void Renderer::Initialize(const RendererDesc& desc)
{
    m_rendererDesc = desc;
    m_backend->Initialize(desc);

    m_renderThread = std::make_unique<RenderThread>();
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

void Renderer::SubmitFrame(FramePacket sceneRenderData)
{
    m_renderThread->Submit(std::move(sceneRenderData));
}

MeshHandle Renderer::CreateMesh(const MeshData& mesh)
{
    return m_backend->CreateMesh(mesh);
}

efg::MaterialHandle Renderer::RegisterMatieral(const efg::MaterialDesc& mat)
{
    return m_backend->RegisterMaterial(mat);
}
