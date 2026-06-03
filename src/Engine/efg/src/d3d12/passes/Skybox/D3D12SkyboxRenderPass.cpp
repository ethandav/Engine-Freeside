#include "..\..\..\..\include\d3d12\passes\Skybox\D3D12SkyboxRenderPass.h"
#include "..\..\..\..\include\d3d12\resources\D3D12GpuTexture.h"
#include "..\..\..\..\include\d3d12\passes\Skybox\D3D12SkyboxRenderPassTypes.h"
#include "..\..\..\..\..\Freeside.Core\include\shapes\skybox.h"
#include "..\..\..\..\include\d3d12\systems\D3D12ResourceSystem.h"

namespace efg::d3d12
{
	void D3D12SkyboxRenderPass::Initialize(D3D12ResourceSystem* resources)
	{
		Freeside::MeshData skybox = Freeside::Shapes::SkyboxCube().mesh;
		Freeside::MeshHandle hSkybox = resources->CreateMesh(skybox);
		m_skyboxMesh = resources->Meshes().Get(hSkybox);
	}

	void D3D12SkyboxRenderPass::Execute(D3D12PassContext& ctx, const FramePacket& scene)
	{
		ID3D12DescriptorHeap* heaps[] =
		{
			ctx.services->descriptors->GetCBVSRVUAVHeap()
		};
		ctx.frameContext->commandContext->SetDescriptorHeaps(_countof(heaps), heaps);
		ctx.frameContext->commandContext->BindPipeline(ctx.services->pipelines->GetPipelineById(PipelineId::Skybox));

		const GpuTextureCube skyboxTexture = scene.skyboxTexture.IsValid() ? ctx.libraries->textures->GetTextureCubeByHandle(scene.skyboxTexture) : ctx.libraries->textures->GetDefaulSkyboxTexture();

		Freeside::Math::Mat4 view = scene.camera.GetViewMatrix();

		// Remove translation from the view matrix.
		view.m[0][3] = 0.0f;
		view.m[1][3] = 0.0f;
		view.m[2][3] = 0.0f;

		Freeside::Math::Mat4 skyboxViewProjection = scene.camera.GetProjectionMatrix() * view;

		GpuSkyboxConstants constants = {};
		constants.ViewProjection = Freeside::Math::Transpose(skyboxViewProjection);
		constants.SkyboxTextureDescriptorIndex = skyboxTexture.bindlessSrvIndex;

		D3D12_GPU_VIRTUAL_ADDRESS skyboxCB = ctx.services->buffers->CopyToConstantBufferArena(ctx.frameContext->frameResource->constantBufferArena, &constants, sizeof(GpuSkyboxConstants));
		ctx.frameContext->commandContext->SetGraphicsRootConstantBufferView(0, skyboxCB);
		
		ctx.frameContext->commandContext->DrawMeshInstanced(m_skyboxMesh, 1);
	}

	void D3D12SkyboxRenderPass::SetSkyboxMesh(GpuMesh mesh)
	{
		m_skyboxMesh = mesh;
	}
}