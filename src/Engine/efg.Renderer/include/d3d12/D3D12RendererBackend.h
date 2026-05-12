#pragma once
#include "..\render\IRendererBackend.h"
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
#include "D3D12MaterialLibrary.h"

namespace efg::d3d12
{
	struct FrameResource
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		UINT64 fenceValue = 0;
		GpuConstantBuffer cameraConstantBuffer = {};
		GpuConstantBuffer directionalLightConstantBuffer = {};
		GpuConstantBuffer pointLightConstantBuffer = {};
		GpuConstantBufferArena objectConstantArena = {};
		GpuConstantBufferArena materialConstantArena = {};
		GpuStructuredBuffer pointLightStructuredBuffer = {};
		GpuUploadBufferArena gpuUploadBufferArena = {};
		GpuDepthBuffer depthBuffer = {};
	};

	struct FrameContext
	{
		UINT frameIndex = 0;
		FrameResource* frame = nullptr;
		ID3D12GraphicsCommandList* commandList = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle = {};
		ID3D12Resource* backBuffer = nullptr;
	};

	class D3D12RendererBackend final : public IRendererBackend
	{
	public:
		void Initialize(const RendererDesc& desc) override;
		void Shutdown() override;
		void Render(const FramePacket& scene) override;
		efg::MeshHandle CreateMesh(const efg::MeshData& mesh) override;
		efg::MaterialHandle RegisterMaterial(const efg::MaterialDesc& mat) override;

	private:
		FrameContext BeginFrame();
		void UpdateFrameConstants(const FrameContext& ctx, const FramePacket& scene);
		void UpdatePointLights(const FrameContext& ctx, const FramePacket& scene);
		void ProcessUploads();
		void FlushPendingUploads();
		void EndFrame(const FrameContext& ctx);
		void RecordBackBufferSetup(const FrameContext& ctx);
		void RecordForwardLitGeometryPass(const FrameContext& ctx, const FramePacket& scene);
		void BindPipeline(ID3D12GraphicsCommandList* commandList, PipelineId pipelineId);
		void DrawMesh(ID3D12GraphicsCommandList* commandList, efg::MeshHandle handle);
		void DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, efg::MeshHandle handle, uint32_t instanceCount);
		void DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene);
		void SortRenderObjectsForInstancing(std::vector<uint32_t>& sortedIndices, const FramePacket& scene);
		void DrawInstancedBatch(const FrameContext& ctx, const FramePacket& scene, const std::vector<uint32_t>& sortedIndices, uint32_t begin, uint32_t end);


		static constexpr UINT NumFramesInFlight = 2;

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;

		D3D12Context m_graphicsContext = {};
		D3D12DirectCommandContext m_commandContext = {};
		D3D12DescriptorContext m_descriptorContext = {};
		D3D12UploadContext m_uploadContext = {};
		D3D12GraphicsPipelineLibary m_graphicsPipelineLibrary = {};
		D3D12ShaderLibrary m_shaderLibrary = {};
		D3D12MaterialLibrary m_materialLibrary = {};
		D3D12BufferFactory m_bufferFactory = {};
		D3D12MeshLibrary m_meshLibrary;
		D3D12SwapChain m_swapChain = {};
		D3D12QueueFence m_directFence = {};

		std::array<FrameResource, NumFramesInFlight> m_frameResources = {};
	};
}