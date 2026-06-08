#include "..\..\..\include\d3d12\systems\D3D12FrameSystem.h"
#include "..\..\..\include\d3d12\systems\D3D12DeviceSystem.h"
#include "..\..\..\include\d3d12\systems\D3D12ResourceSystem.h"
#include "..\..\..\include\d3d12\types\D3D12DrawTypes.h"

namespace efg::d3d12
{
	void D3D12FrameSystem::Initialize(D3D12DeviceSystem* device, D3D12ResourceSystem* resources, uint32_t width, uint32_t height)
	{
		m_device = device;
		m_resources = resources;

		CreateViewportAndScissor(width, height);
		CreateFrameResources();
	}

	void D3D12FrameSystem::Shutdown()
	{
		DestroyFrameResources();
	}

	FrameContext D3D12FrameSystem::BeginFrame(RenderQueue* renderQueue)
	{
		uint64_t frameIndex = m_device->SwapChain().GetFrameIndex();
		FrameResource& frame = m_frameResources[frameIndex];

		m_device->DirectFence().WaitForCPU(frame.fenceValue);
		m_device->DirectCommandContext().BeginRecording(frame.commandAllocator.Get());

		frame.ResetTransientAllocators();

		FrameContext ctx = {};
		ctx.frameIndex = frameIndex;
		ctx.frameResource = &frame;
		ctx.commandContext = &m_device->DirectCommandContext();
		ctx.graphicsContext = &m_device->GraphicsContext();
		ctx.backBuffer = m_device->SwapChain().GetCurrentBackBuffer();
		ctx.backBufferHandle = m_device->SwapChain().GetCurrentBackBufferHandle();
		ctx.renderQueue = renderQueue;

		return ctx;
	}

	void D3D12FrameSystem::EndFrame(const FrameContext& ctx, const RenderTargets& renderTargets)
	{
		D3D12DirectCommandContext& commandContext = m_device->DirectCommandContext();
		D3D12SwapChain& swapChain = m_device->SwapChain();
		D3D12QueueFence& fence = m_device->DirectFence();

		ID3D12CommandQueue* queue = commandContext.GetDirectCommandQueue();
		commandContext.ResourceBarrierTransition(renderTargets.sceneColor.resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
		commandContext.ResourceBarrierTransition(ctx.backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandContext.EndRecording();
		commandContext.ExecuteDirect();

		ctx.frameResource->fenceValue = fence.Signal(queue);

		swapChain.Present();
	}

	void D3D12FrameSystem::RecordBackBufferSetup(const FrameContext& ctx, const RenderTargets& renderTargets)
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_device->DirectCommandContext().SetViewportAndScissor(m_viewport, m_scissorRect);
		m_device->DirectCommandContext().ResourceBarrierTransition(ctx.backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_device->DirectCommandContext().SetRenderTarget(1, &ctx.backBufferHandle, nullptr);
		m_device->DirectCommandContext().ClearRenderTarget(ctx.backBufferHandle, clearColor);
	}

	void D3D12FrameSystem::RecordSceneColorBufferSetup(const FrameContext& ctx, const RenderTargets& renderTargets)
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_device->DirectCommandContext().SetViewportAndScissor(m_viewport, m_scissorRect);
		m_device->DirectCommandContext().ResourceBarrierTransition(renderTargets.sceneColor.resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_device->DirectCommandContext().SetRenderTarget(1, &renderTargets.sceneColor.rtv, &renderTargets.sceneDepth.dsv);
		m_device->DirectCommandContext().ClearRenderTarget(renderTargets.sceneColor.rtv, clearColor);
		m_device->DirectCommandContext().ClearDepthStencil(renderTargets.sceneDepth.dsv, 1.0f, 0);
	}

	void D3D12FrameSystem::CreateViewportAndScissor(uint32_t width, uint32_t height)
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(width);
		m_viewport.Height = static_cast<float>(height);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_scissorRect.left = 0;
		m_scissorRect.top = 0;
		m_scissorRect.right = static_cast<LONG>(width);
		m_scissorRect.bottom = static_cast<LONG>(height);
	}

	void D3D12FrameSystem::CreateFrameResources()
	{
		for (UINT i = 0; i < NumFramesInFlight; i++)
		{
			m_frameResources[i].commandAllocator = m_device->DirectCommandContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
			m_frameResources[i].uploadBufferArena = m_resources->BufferFactory().CreateUploadBufferArena(100000 * sizeof(InstanceData));
			m_frameResources[i].constantBufferArena = m_resources->BufferFactory().CreateConstantBufferArena(ConstantArenaSize);
			m_frameResources[i].descriptorArena = m_device->DescriptorContext().CreateFrameDescriptorArena(i);
		}
	}

	void D3D12FrameSystem::DestroyFrameResources()
	{
		for (UINT i = 0; i < NumFramesInFlight; i++)
		{
			m_resources->BufferFactory().DestroyUploadBufferArena(m_frameResources[i].uploadBufferArena);
			m_resources->BufferFactory().DestroyConstantBufferArena(m_frameResources[i].constantBufferArena);
		}
	}
}