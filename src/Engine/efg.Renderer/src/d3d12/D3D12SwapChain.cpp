#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <d3dx12.h>

#include "..\..\include\Renderer.h"

void Renderer::CreateSwapChain(void* nativeWindowHandle, uint32_t width, uint32_t height)
{
    ID3D12CommandQueue* queue = m_commandContext.GetCommandQueue();
    ComPtr<IDXGISwapChain1> swapChain;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    D3D12_THROW_IF_FAILED(m_graphicsContext.GetFactory()->CreateSwapChainForHwnd(
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

void Renderer::CreateRenderTargetViews()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT n = 0; n < FrameCount; n++)
    {
        D3D12_THROW_IF_FAILED(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
        m_graphicsContext.GetDevice()->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, m_rtvDescriptorSize);
    }
}