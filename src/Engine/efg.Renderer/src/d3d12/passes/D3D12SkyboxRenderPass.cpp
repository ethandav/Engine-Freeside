#include "..\..\..\include\d3d12\passes\Skybox\D3D12SkyboxRenderPass.h"
#include "..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\..\..\include\d3d12\descriptors\D3D12DescriptorContext.h"
#include "..\..\..\include\d3d12\libraries\D3D12MaterialTextureLibrary.h"
#include "..\..\..\include\d3d12\factories\D3D12BufferFactory.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuTexture.h"
#include "..\..\..\include\d3d12\passes\Skybox\D3D12SkyboxRenderPassTypes.h"

namespace efg::d3d12
{
	void D3D12SkyboxRenderPass::Initialize(D3D12GraphicsPipelineLibrary* pipelineLibrary, D3D12DescriptorContext* descriptorCtx, D3D12MaterialTextureLibrary* textureLibrary, D3D12BufferFactory* bufferFactory)
	{
		m_pipelineLibrary = pipelineLibrary;
		m_descriptorContext = descriptorCtx;
		m_textureLibrary = textureLibrary;
	}

	void D3D12SkyboxRenderPass::Execute(const FrameContext& ctx, const FramePacket& scene)
	{
		ID3D12DescriptorHeap* heaps[] =
		{
			m_descriptorContext->GetCBVSRVUAVHeap()
		};
		ctx.commandContext->SetDescriptorHeaps(_countof(heaps), heaps);
		ctx.commandContext->BindPipeline(m_pipelineLibrary->Get(PipelineId::Skybox));

		const GpuTextureCube skyboxTexture = scene.skyboxTexture.IsValid() ? m_textureLibrary->GetTextureCubeByHandle(scene.skyboxTexture) : m_textureLibrary->GetDefaulSkyboxTexture();

		Freeside::Math::Mat4 view = scene.camera.GetViewMatrix();

		// Remove translation from the view matrix.
		view.m[0][3] = 0.0f;
		view.m[1][3] = 0.0f;
		view.m[2][3] = 0.0f;

		Freeside::Math::Mat4 skyboxViewProjection = scene.camera.GetProjectionMatrix() * view;

		GpuSkyboxConstants constants = {};
		constants.ViewProjection = Freeside::Math::Transpose(skyboxViewProjection);
		constants.SkyboxTextureDescriptorIndex = skyboxTexture.bindlessSrvIndex;

		D3D12_GPU_VIRTUAL_ADDRESS skyboxCB = m_bufferFactory->CopyToConstantBufferArena(ctx.frame->constantBufferArena, &constants, sizeof(GpuSkyboxConstants));
		ctx.commandContext->SetGraphicsRootConstantBufferView(0, skyboxCB);
		
		ctx.commandContext->DrawMeshInstanced(m_skyboxMesh, 1);
	}

	void D3D12SkyboxRenderPass::SetSkyboxMesh(GpuMesh mesh)
	{
		m_skyboxMesh = mesh;
	}
}