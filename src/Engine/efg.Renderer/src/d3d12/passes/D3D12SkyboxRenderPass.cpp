#include "..\..\..\include\d3d12\passes\Skybox\D3D12SkyboxRenderPass.h"
#include "..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"

namespace efg::d3d12
{
	void D3D12SkyboxRenderPass::Initialize(D3D12GraphicsPipelineLibrary* pipelineLibrary, D3D12DescriptorContext* descriptorCtx)
	{
		m_pipelineLibrary = pipelineLibrary;
		m_descriptorContext = descriptorCtx;
	}

	void D3D12SkyboxRenderPass::Execute(const FrameContext& ctx, const FramePacket& scene)
	{
		ID3D12DescriptorHeap* heaps[] =
		{
			m_descriptorContext->GetCBVSRVUAVHeap()
		};
		ctx.commandContext->SetDescriptorHeaps(_countof(heaps), heaps);
		ctx.commandContext->BindPipeline(m_pipelineLibrary->Get(PipelineId::Skybox));
	}
}