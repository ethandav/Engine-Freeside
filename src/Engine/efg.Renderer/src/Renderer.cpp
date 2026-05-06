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

void Renderer::BeginFrame(efg::Camera* camera)
{
    m_backend->BeginFrame(camera);
}

void Renderer::EndFrame()
{
    m_backend->EndFrame();
}

void Renderer::AddRenderObjectToRenderQueue(const RenderObject& object)
{
    m_backend->AddRenderObjectToRenderQueue(object);
}

MeshHandle Renderer::CreateMesh(const MeshData& mesh)
{
    return m_backend->CreateMesh(mesh);
}