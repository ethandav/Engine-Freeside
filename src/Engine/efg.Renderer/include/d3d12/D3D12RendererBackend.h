#pragma once
#include "..\IRendererBackend.h"
#include "D3D12Context.h"
#include "D3D12Error.h"
#include "D3D12GraphicsPipelineLibrary.h"
#include "D3D12ShaderLibrary.h"
#include "D3D12BufferFactory.h"
#include "D3D12MeshLibrary.h"
#include "D3D12CommandContext.h"
#include "D3D12SwapChain.h"
#include "D3D12QueueFence.h"
#include "D3D12UploadContext.h"

class D3D12RendererBackend final : public IRendererBackend
{
public:
	void Initialize(const RendererDesc& desc) override;
	void Shutdown() override;
	void BeginFrame() override;
	void EndFrame() override;
	Engine::MeshHandle UploadMesh(const Engine::MeshData& mesh) override;
	void DrawMesh(Engine::MeshHandle handle) override;

private:
	static constexpr UINT NumFramesInFlight = 2;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	D3D12Context m_graphicsContext = {};
	D3D12DirectCommandContext m_commandContext = {};
	D3D12DescriptorContext m_descriptorContext = {};
	D3D12UploadContext m_uploadContext = {};
	D3D12GraphicsPipelineLibary m_graphicsPipelineLibrary;
	D3D12ShaderLibrary m_shaderLibrary;
	D3D12BufferFactory m_bufferFactory;
	D3D12MeshLibrary m_meshLibrary;
	D3D12SwapChain m_swapChain = {};
	D3D12QueueFence m_directFence = {};

	struct FrameResource
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		UINT64 fenceValue = 0;
	};

	std::array<FrameResource, NumFramesInFlight> m_frameResources = {};
};