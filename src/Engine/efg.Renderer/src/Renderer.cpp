#include "..\include\Renderer.h"

void Renderer::Initialize(const RendererDesc& desc)
{
    CreateFactory();
    CreateDevice();
    CreateCommandObjects();
    CreateSwapChain(desc.nativeWindowHandle, desc.width, desc.height);
    CreateDescriptorHeaps();
    CreateRenderTargetViews();
    CreateFence();

    m_shaderLibrary.Initialize();
    m_graphicsPipelineLibrary.Initialize(m_device.Get(), m_shaderLibrary);

/*
    CreateRootSignature();
    CreateShaders();
    CreatePipelineState();
    CreateTriangleVertexBuffer();
*/
}

