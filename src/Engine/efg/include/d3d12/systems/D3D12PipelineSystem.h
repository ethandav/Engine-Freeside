#pragma once
#include "..\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\libraries\D3D12ShaderLibrary.h"
#include "..\factories\D3D12PipelineFactory.h"
#include "..\factories\D3D12RootSignatureFactory.h"

namespace efg::d3d12
{
	class D3D12PipelineSystem
	{
	public:
		void Initialize(ID3D12Device* device);
		D3D12GraphicsPipelineLibrary& GraphicsPipelineLibrary();
	private:
		D3D12GraphicsPipelineLibrary m_graphicsPipelineLibrary = {};
		D3D12ShaderLibrary m_shaderLibrary = {};
		D3D12PipelineFactory m_pipelineFactory = {};
		D3D12RootSignatureFactory m_rootSignatureFactory = {};
	};
}