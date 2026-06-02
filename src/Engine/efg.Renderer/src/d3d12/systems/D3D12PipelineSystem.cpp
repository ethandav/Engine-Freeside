#include "..\..\..\include\d3d12\systems\D3D12PipelineSystem.h"
#include "..\..\..\include\d3d12\passes\D3D12Pipelines.h"

namespace efg::d3d12
{
	void D3D12PipelineSystem::Initialize(ID3D12Device* device)
	{
		m_shaderLibrary.Initialize();
		m_pipelineFactory.Initialize(device);
		m_rootSignatureFactory.Initialize(device);
		m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::ForwardLitGeometry, ForwardLitGeometryPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
		m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::ShadowMap, ShadowMapPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
		m_graphicsPipelineLibrary.AddGraphicsPipeline(PipelineId::Skybox, SkyboxPipeline::CreatePipeline(m_pipelineFactory, m_rootSignatureFactory, m_shaderLibrary));
	}

	D3D12GraphicsPipelineLibrary& D3D12PipelineSystem::GraphicsPipelineLibrary()
	{
		return m_graphicsPipelineLibrary;
	}
}