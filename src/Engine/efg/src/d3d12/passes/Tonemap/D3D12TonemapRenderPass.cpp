#include "..\..\..\..\include\d3d12\passes\Tonemap\D3D12TonemapRenderPass.h"

namespace efg::d3d12
{
	void D3D12TonemapRenderPass::Execute(const D3D12PassContext& ctx, const GpuTexture2D& sceneColor)
	{
        ID3D12DescriptorHeap* heaps[] =
        {
            ctx.services->descriptors->GetCBVSRVUAVHeap()
        };
        ctx.frameContext->commandContext->SetDescriptorHeaps(_countof(heaps), heaps);
        ctx.frameContext->commandContext->BindPipeline(ctx.services->pipelines->GetPipelineById(PipelineId::Tonemap));
        ctx.frameContext->commandContext->SetGraphicsRootDescriptorTable(0, sceneColor.gpuSrv);
        ctx.frameContext->commandContext->DrawInstanced(3, 1, 0, 0);
	}
}