#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <d3dx12.h>

#include "..\..\include\d3d12\D3D12SwapChain.h"
#include "..\..\include\d3d12\D3D12Error.h"

void D3D12SwapChain::Initialize(D3D12Context* graphicsContext, D3D12DirectCommandContext* commandContext, D3D12DescriptorContext* descriptorContext)
{
    m_graphicsContext = graphicsContext;
    m_commandContext = commandContext;
    m_descriptorContext = descriptorContext;
}

void D3D12SwapChain::CreateSwapChain(void* nativeWindowHandle, uint32_t width, uint32_t height)
{
    ID3D12CommandQueue* queue = m_commandContext->GetDirectCommandQueue();
    ComPtr<IDXGISwapChain1> swapChain;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    D3D12_THROW_IF_FAILED(m_graphicsContext->GetFactory()->CreateSwapChainForHwnd(
        queue,
        static_cast<HWND>(nativeWindowHandle),
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    D3D12_THROW_IF_FAILED(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void D3D12SwapChain::CreateBackBufferViews()
{
    for (uint32_t i = 0; i < FrameCount; ++i)
    {
        D3D12_THROW_IF_FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf())));
        m_backBufferHandles[i] = m_descriptorContext->CreateRTV(m_backBuffers[i].Get(), nullptr);
    }
}

UINT D3D12SwapChain::GetFrameIndex()
{
    return m_frameIndex;
}

ID3D12Resource* D3D12SwapChain::GetCurrentBackBuffer() const
{
    return m_backBuffers[m_frameIndex].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12SwapChain::GetCurrentBackBufferHandle() const
{
    return m_backBufferHandles[m_frameIndex].cpu;
}

void D3D12SwapChain::Present()
{
    D3D12_THROW_IF_FAILED(m_swapChain->Present(1, 0));
    UpdateFrameIndex();
}

void D3D12SwapChain::UpdateFrameIndex()
{
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}