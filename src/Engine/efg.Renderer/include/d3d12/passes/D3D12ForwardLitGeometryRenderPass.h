#pragma once
#include "..\D3D12RenderData.h"
#include "..\D3D12GraphicsPipelineLibrary.h"
#include "..\D3D12DescriptorContext.h"
#include "..\D3D12MeshLibrary.h"
#include "..\D3D12MaterialLibrary.h"
#include "..\D3D12BufferFactory.h"
#include "..\..\render\RenderTypes.h"

namespace efg::d3d12
{
	class D3D12ForwardLitGeometryRenderPass
	{
	public:
		void Initialize(D3D12GraphicsPipelineLibary* pipelineLib, D3D12DescriptorContext* descriptorCtx, D3D12MeshLibrary* meshLibrary, D3D12MaterialLibrary* materialLibrary, D3D12BufferFactory* bufferFactory);
		void Execute(const FrameContext& ctx, const FramePacket& scene);
	private:
		struct ForwardLitPassResources
		{
			D3D12_GPU_VIRTUAL_ADDRESS cameraCB = 0;
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightCB = 0;
			D3D12_GPU_VIRTUAL_ADDRESS pointLightConstantsCB = 0;
			D3D12_GPU_VIRTUAL_ADDRESS pointLightsSRV = 0;
			Lights::PointLightConstants pointLightConstants = {};
		};
		void BeginPass(const FrameContext& ctx, const FramePacket& scene);
		void UploadPassResources(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources);
		void BindPassResources(const FrameContext& ctx, ForwardLitPassResources& resources);
		void UploadPointLights(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources);
		void UploadFrameConstants(const FrameContext& ctx, const FramePacket& scene, ForwardLitPassResources& resources);
		void DrawAllRenderObjects(const FrameContext& ctx, const FramePacket& scene);
		void SortRenderObjectsForInstancing(std::vector<uint32_t>& sortedIndices, const FramePacket& scene);
		void DrawInstancedBatch(const FrameContext& ctx, const FramePacket& scene, const std::vector<uint32_t>& sortedIndices, uint32_t begin, uint32_t end);
		void DrawMeshInstanced(ID3D12GraphicsCommandList* commandList, MeshHandle handle, uint32_t instanceCount);

		D3D12GraphicsPipelineLibary* m_pipelineLibrary = nullptr;
		D3D12DescriptorContext* m_descriptorContext = nullptr;
		D3D12MeshLibrary* m_meshLibrary = nullptr;
		D3D12MaterialLibrary* m_materialLibrary = nullptr;
		D3D12BufferFactory* m_bufferFactory = nullptr;
	};
}