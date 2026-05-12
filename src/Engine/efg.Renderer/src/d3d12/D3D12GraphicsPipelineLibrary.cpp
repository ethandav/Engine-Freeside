#include "..\..\include\d3d12\D3D12GraphicsPipelineLibrary.h"
#include <d3dx12.h>

namespace efg::d3d12
{
    void D3D12GraphicsPipelineLibary::Initialize(D3D12Context* device, const D3D12ShaderLibrary& shaderLibrary)
    {
        m_graphicsContext = device;
        CreateForwarLitGeometryPipeline(shaderLibrary);
    }

    const GraphicsPipelineState& D3D12GraphicsPipelineLibary::Get(PipelineId id) const
    {
        const auto index = static_cast<size_t>(id);

        if (index >= m_graphicsPipelines.size())
        {
            throw std::runtime_error("Invalid PipelineId.");
        }

        return m_graphicsPipelines[index];
    }

    void D3D12GraphicsPipelineLibary::CreateRootSignature(ID3D12RootSignature** rootSignature)
    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        CD3DX12_ROOT_PARAMETER rootParameters[7] = {};
        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;

        rootParameters[0].InitAsConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Camera), 0, D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[1].InitAsConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Object), 0, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[2].InitAsConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::DirectionalLight), 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[3].InitAsConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::Material), 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[4].InitAsConstantBufferView(static_cast<UINT>(ForwardLitRootParameter::PointLightConstants), 0, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_DESCRIPTOR_RANGE pointLightSrvRange;
        pointLightSrvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        rootParameters[5].InitAsDescriptorTable(1, &pointLightSrvRange, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[6].InitAsShaderResourceView(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

        rootSignatureDesc.Init(
            _countof(rootParameters),
            rootParameters,
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );

        D3D12_THROW_IF_FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature)));
    }

    void D3D12GraphicsPipelineLibary::AddGraphicsPipeline(PipelineId id, GraphicsPipelineState pipeline)
    {
        const size_t index = static_cast<size_t>(id);

        if (index >= m_graphicsPipelines.size())
        {
            throw std::runtime_error("Invalid pipeline id.");
        }

        m_graphicsPipelines[index] = std::move(pipeline);
    }

    void D3D12GraphicsPipelineLibary::CreateForwarLitGeometryPipeline(const D3D12ShaderLibrary& shaderLibrary)
    {
        GraphicsPipelineState pipeline = {};
        CreateRootSignature(pipeline.rootSignature.GetAddressOf());
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        const ShaderBytecode& vertexShader = shaderLibrary.Get(ShaderId::GeometryVS);
        const ShaderBytecode& pixelShader = shaderLibrary.Get(ShaderId::GeometryPS);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = pipeline.rootSignature.Get();
        psoDesc.VS = vertexShader.GetD3D12Bytecode();
        psoDesc.PS = pixelShader.GetD3D12Bytecode();
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipeline.pipelineState)));

        AddGraphicsPipeline(PipelineId::ForwardLitGeometry, pipeline);
    }
}