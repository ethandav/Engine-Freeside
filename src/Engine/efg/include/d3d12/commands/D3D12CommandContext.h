#pragma once

#include "..\resources\D3D12GpuMesh.h"
#include "..\types\D3D12PipelineTypes.h"

#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <vector>

namespace efg::d3d12
{
	class D3D12DeviceContext;

	class D3D12DirectCommandContext
	{
	public:
		void Initialize(D3D12DeviceContext* graphicsContext);
		void BeginRecording(ID3D12CommandAllocator* commandAllocator);
		void EndRecording();
		ID3D12GraphicsCommandList* GetDirectCommandList();
		ID3D12CommandQueue* GetDirectCommandQueue();
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type);
		void ResourceBarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		void ExecuteDirect();
		void ExecuteCopy();
		void SetViewportAndScissor(const D3D12_VIEWPORT& m_viewport, const D3D12_RECT& m_scissorRect);
		void SetRenderTarget(uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle);
		void SetGraphicsRootConstantBufferView(const uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
		void SetGraphicsRootShaderResourceView(const uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
		void SetGraphicsRootDescriptorTable(const uint32_t rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle);
		void SetDescriptorHeaps(uint32_t numDescriptorHeaps, ID3D12DescriptorHeap* heaps[]);
		void ClearRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, const float clearColor[]);
		void ClearDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, float depth, uint8_t stencil);
		void BindPipeline(const GraphicsPipelineState& pipeline);
		void DrawMeshInstanced(const GpuMesh& mesh, uint32_t instanceCount);
		void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startIndexLocation);
		void QueueBarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		void FlushPendingBarrierTransitions();
	private:
		void CreateCommandObjects();
		void CreateCommandList(ID3D12GraphicsCommandList** list, ID3D12CommandAllocator*, D3D12_COMMAND_LIST_TYPE type);
		void CreateCommandQueue(ID3D12CommandQueue** queue, D3D12_COMMAND_LIST_TYPE type);

		struct PendingBarrierTransition
		{
			ID3D12Resource* pResource = nullptr;
			D3D12_RESOURCE_STATES before = {};
			D3D12_RESOURCE_STATES after = {};
		};

		D3D12DeviceContext* m_graphicsContext = {};
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_directCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_directQueue;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_directCommandList;

		std::vector<PendingBarrierTransition> m_pendingBarrierTransitions = {};
	};
}