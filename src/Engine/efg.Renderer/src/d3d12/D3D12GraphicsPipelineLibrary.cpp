#include "..\..\include\d3d12\D3D12GraphicsPipelineLibrary.h"
#include <d3dx12.h>

void GraphicsPipelineLibary::Initialize(D3D12Context* device, const ShaderLibrary& shaderLibrary)
{
    m_graphicsContext = device;
    CreateTrianglePipeline(shaderLibrary);
}

const GraphicsPipelineState& GraphicsPipelineLibary::Get(PipelineId id) const
{
    const auto index = static_cast<size_t>(id);

    if (index >= m_graphicsPipelines.size())
    {
        throw std::runtime_error("Invalid PipelineId.");
    }

    return m_graphicsPipelines[index];
}

void GraphicsPipelineLibary::CreateRootSignature(ID3D12RootSignature** rootSignature)
{
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    D3D12_THROW_IF_FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature)));
}

void GraphicsPipelineLibary::AddGraphicsPipeline(PipelineId id, GraphicsPipelineState pipeline)
{
    const size_t index = static_cast<size_t>(id);

    if (index >= m_graphicsPipelines.size())
    {
        throw std::runtime_error("Invalid pipeline id.");
    }

    m_graphicsPipelines[index] = std::move(pipeline);
}

void GraphicsPipelineLibary::CreateTrianglePipeline(const ShaderLibrary& shaderLibrary)
{
    GraphicsPipelineState pipeline = {};
    CreateRootSignature(pipeline.rootSignature.GetAddressOf());
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    const ShaderBytecode& vertexShader = shaderLibrary.Get(ShaderId::TriangleVS);

    const ShaderBytecode& pixelShader = shaderLibrary.Get(ShaderId::TrianglePS);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = pipeline.rootSignature.Get();
    psoDesc.VS = vertexShader.GetD3D12Bytecode();
    psoDesc.PS = pixelShader.GetD3D12Bytecode();
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipeline.pipelineState)));

    AddGraphicsPipeline(PipelineId::Triangle, pipeline);
}