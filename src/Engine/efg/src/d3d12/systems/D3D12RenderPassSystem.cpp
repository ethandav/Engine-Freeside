#include "..\..\..\include\d3d12\systems\D3D12RenderPassSystem.h"
#include "..\..\..\include\d3d12\systems\D3D12DeviceSystem.h"
#include "..\..\..\include\d3d12\systems\D3D12ResourceSystem.h"
#include "..\..\..\include\d3d12\systems\D3D12PipelineSystem.h"
#include "..\..\..\include\d3d12\systems\D3D12FrameSystem.h"
#include "..\..\..\include\d3d12\frame\D3D12RenderTargets.h"
#include "..\..\..\include\d3d12\core\D3D12Pix.h"

namespace efg::d3d12
{
	void D3D12RenderPassSystem::Initialize(D3D12DeviceSystem* device, D3D12ResourceSystem* resources, D3D12PipelineSystem* pipeline, D3D12FrameSystem* frame, RenderTargets* renderTargets)
	{
		m_device = device;
		m_resources = resources;
		m_pipeline = pipeline;
		m_frame = frame;
		m_renderTargets = renderTargets;

		m_shadowSystem.Initialize(&m_resources->TextureFactory());
		m_skyboxRenderPass.Initialize(m_resources);

		m_renderServices.buffers = &m_resources->BufferFactory();
		m_renderServices.descriptors = &m_device->DescriptorContext();
		m_renderServices.pipelines = &m_pipeline->GraphicsPipelineLibrary();

		m_renderResources.materials = &m_resources->Materials();
		m_renderResources.meshes = &m_resources->Meshes();
		m_renderResources.textures = &m_resources->MaterialTextures();
	}

	void D3D12RenderPassSystem::Execute(const FramePacket& scene, FrameContext& frameCtx, RenderQueue& renderQueue)
	{
		D3D12PassContext passCtx = BuildPassContext(frameCtx, renderQueue);

		ID3D12GraphicsCommandList* commandList = frameCtx.commandContext->GetDirectCommandList();

		PIXBeginEvent(commandList, PIX_COLOR(0, 0, 0), L"Begin Frame");

		m_resources->ProcessUploads(&m_device->DirectCommandContext());

		renderQueue.BuildForwardGeometryBatches(scene.renderObjects);

		PIXBeginEvent(PixColors::ShadowMapPass, L"Shadow System Update");
		ShadowMapFrameData shadowMapFrameData = m_shadowSystem.Update(scene);
		PIXEndEvent();

		PIXBeginEvent(commandList, PixColors::ShadowMapPass, L"Shadow Map Pass");
		m_shadowMapRenderPass.Execute(passCtx, scene, shadowMapFrameData);
		PIXEndEvent(commandList);

		m_device->DirectCommandContext().FlushPendingBarrierTransitions();

		PIXBeginEvent(commandList, PixColors::BackbufferSetup, L"BackBuffer Setup");
		m_frame->RecordBackBufferSetup(frameCtx, *m_renderTargets);
		PIXEndEvent(commandList);

		PIXBeginEvent(commandList, PixColors::SkyboxPass, L"Skybox Render Pass");
		m_skyboxRenderPass.Execute(passCtx, scene);
		PIXEndEvent(commandList);

		PIXBeginEvent(commandList, PixColors::ForwardLitPass, L"Forward Lit Geometry Pass");
		m_forwardLitGeometryRenderPass.Execute(passCtx, scene, shadowMapFrameData);
		PIXEndEvent(commandList);

		m_device->DirectCommandContext().FlushPendingBarrierTransitions();

		PIXEndEvent(commandList);
	}

	D3D12PassContext D3D12RenderPassSystem::BuildPassContext(FrameContext& frameCtx, RenderQueue& renderQueue)
	{
		D3D12PassContext passCtx = {};
		passCtx.frameContext = &frameCtx;
		passCtx.services = &m_renderServices;
		passCtx.libraries = &m_renderResources;
		passCtx.renderQueue = &renderQueue;
		return passCtx;
	}
}