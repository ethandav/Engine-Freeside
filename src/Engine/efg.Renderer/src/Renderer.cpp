#include "..\include\Renderer.h"
#include "..\include\IRendererBackend.h"
#include "..\include\d3d12\D3D12RendererBackend.h"

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
    m_backend->Initialize(desc);
}

void Renderer::Shutdown()
{
    if (m_backend)
    {
        m_backend->Shutdown();
        m_backend.reset();
    }
}

void Renderer::BeginFrame()
{
    m_backend->BeginFrame();
}

void Renderer::EndFrame()
{
    m_backend->EndFrame();
}

MeshHandle Renderer::UploadMesh(const MeshData& mesh)
{
    return m_backend->UploadMesh(mesh);
}

void Renderer::DrawMesh(MeshHandle handle)
{
    m_backend->DrawMesh(handle);
}