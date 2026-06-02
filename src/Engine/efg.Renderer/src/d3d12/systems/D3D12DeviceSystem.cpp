#include "..\..\..\include\d3d12\systems\D3D12DeviceSystem.h"

namespace efg::d3d12
{
	void D3D12DeviceSystem::Initialize(const Freeside::RendererDesc& desc)
	{
		m_graphicsContext.Initialize(false);
		m_directCommandContext.Initialize(&m_graphicsContext);
		m_descriptorContext.Initialize(m_graphicsContext.GetDevice());
		m_descriptorFactory.Initialize(m_graphicsContext.GetDevice(), &m_descriptorContext);
		m_swapChain.Initialize(&m_graphicsContext, &m_directCommandContext, &m_descriptorFactory);
		m_directFence.Initialize(&m_graphicsContext);
		m_swapChain.CreateSwapChain(desc.nativeWindowHandle, desc.width, desc.height);
		m_descriptorContext.CreateAllHeaps();
		m_swapChain.CreateBackBufferViews();
		m_directFence.CreateFence(0);

		m_directFence.WaitForGPU(m_directCommandContext.GetDirectCommandQueue());
	}

	D3D12Context& D3D12DeviceSystem::GraphicsContext()
	{
		return m_graphicsContext;
	}

	D3D12DescriptorFactory& D3D12DeviceSystem::DescriptorFactory()
	{
		return m_descriptorFactory;
	}

	D3D12DescriptorContext& D3D12DeviceSystem::DescriptorContext()
	{
		return m_descriptorContext;
	}

	D3D12DirectCommandContext& D3D12DeviceSystem::DirectCommandContext()
	{
		return m_directCommandContext;
	}

	D3D12QueueFence& D3D12DeviceSystem::DirectFence()
	{
		return m_directFence;
	}

	D3D12SwapChain& D3D12DeviceSystem::SwapChain()
	{
		return m_swapChain;
	}
}