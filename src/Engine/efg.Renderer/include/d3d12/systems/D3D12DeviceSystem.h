#pragma once
#include "..\core\D3D12DeviceContext.h"
#include "..\presentation\D3D12SwapChain.h"
#include "..\commands\D3D12CommandContext.h"
#include "..\descriptors\D3D12DescriptorContext.h"
#include "..\factories\D3D12DescriptorFactory.h"
#include "..\commands\D3D12QueueFence.h"
#include "..\..\render\types\RendererDesc.h"

namespace efg::d3d12
{
	class D3D12DeviceSystem
	{
	public:
		void Initialize(const Freeside::RendererDesc& desc);
		D3D12DeviceContext& GraphicsContext();
		D3D12DescriptorFactory& DescriptorFactory();
		D3D12DescriptorContext& DescriptorContext();
		D3D12DirectCommandContext& DirectCommandContext();
		D3D12QueueFence& DirectFence();
		D3D12SwapChain& SwapChain();

	private:
		D3D12DeviceContext m_graphicsContext = {};
		D3D12SwapChain m_swapChain = {};
		D3D12DirectCommandContext m_directCommandContext = {};
		D3D12DescriptorContext m_descriptorContext = {};
		D3D12DescriptorFactory m_descriptorFactory = {};
		D3D12QueueFence m_directFence = {};
	};
}