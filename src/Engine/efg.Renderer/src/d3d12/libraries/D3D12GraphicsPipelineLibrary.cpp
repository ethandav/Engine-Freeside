#include "..\..\..\include\d3d12\libraries\D3D12GraphicsPipelineLibrary.h"
#include "..\..\..\include\d3d12\core\D3D12Context.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"

#include <stdexcept>
#include <d3dx12.h>

namespace efg::d3d12
{
    void D3D12GraphicsPipelineLibary::Initialize(D3D12Context* device, const D3D12ShaderLibrary& shaderLibrary)
    {
        m_graphicsContext = device;
        CreateForwarLitGeometryPipeline(shaderLibrary);
        CreateShadowMapPipeline(shaderLibrary);
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

    void D3D12GraphicsPipelineLibary::CreateForwardLitRootSignature(ID3D12RootSignature** rootSignature)
    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;

        CD3DX12_STATIC_SAMPLER_DESC materialSampler(
            0, // s0
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP
        );
        materialSampler.MaxAnisotropy = 16;

        std::array<CD3DX12_ROOT_PARAMETER, static_cast<size_t>(ForwardLitRootParameter::Count)> rootParameters = {};

        rootParameters[static_cast<UINT>(ForwardLitRootParameter::Camera)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::DirectionalLightMetadata)].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::Material)].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::PointLightMetadata)].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::ShadowMetadata)].InitAsConstantBufferView(5, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::InstanceData)].InitAsShaderResourceView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::PointLightsSrv)].InitAsShaderResourceView(1, 0, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_DESCRIPTOR_RANGE materialTextures = {};
        materialTextures.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 2);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::BaseColorTexture)].InitAsDescriptorTable(1, &materialTextures, D3D12_SHADER_VISIBILITY_PIXEL);

        rootParameters[static_cast<UINT>(ForwardLitRootParameter::DirectionalLightsSrv)].InitAsShaderResourceView(5, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::DirectionalShadowDataSrv)].InitAsShaderResourceView(6, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::PointShadowDataSrv)].InitAsShaderResourceView(7, 0, D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC shadowSampler = {};
        shadowSampler.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        shadowSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        shadowSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        shadowSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        shadowSampler.MipLODBias = 0.0f;
        shadowSampler.MaxAnisotropy = 0;
        shadowSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        shadowSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
        shadowSampler.MinLOD = 0.0f;
        shadowSampler.MaxLOD = D3D12_FLOAT32_MAX;
        shadowSampler.ShaderRegister = 1;
        shadowSampler.RegisterSpace = 0;
        shadowSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        D3D12_STATIC_SAMPLER_DESC staticSamplers[] =
        {
            materialSampler,
            shadowSampler
        };

        D3D12_ROOT_SIGNATURE_FLAGS flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

        rootSignatureDesc.Init(
            static_cast<UINT>(rootParameters.size()),
            rootParameters.data(),
            _countof(staticSamplers),
            staticSamplers,
            flags
        );

        D3D12_THROW_IF_FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature)));
    }

    void D3D12GraphicsPipelineLibary::CreateShadowMapRootSignature(ID3D12RootSignature** rootSignature)
    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        CD3DX12_ROOT_PARAMETER rootParameters[2] = {};
        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;

        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[1].InitAsShaderResourceView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

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
        CreateForwardLitRootSignature(pipeline.rootSignature.GetAddressOf());
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        const ShaderBytecode& vertexShader = shaderLibrary.Get(ShaderId::GeometryVS);
        const ShaderBytecode& pixelShader = shaderLibrary.Get(ShaderId::ForwardLightingPS);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = pipeline.rootSignature.Get();
        psoDesc.VS = vertexShader.GetBytecode();
        psoDesc.PS = pixelShader.GetBytecode();
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

    void D3D12GraphicsPipelineLibary::CreateShadowMapPipeline(const D3D12ShaderLibrary& shaderLibrary)
    {
        GraphicsPipelineState pipeline = {};
        CreateShadowMapRootSignature(pipeline.rootSignature.GetAddressOf());
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        const ShaderBytecode& vertexShader = shaderLibrary.Get(ShaderId::GeometryVS);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = pipeline.rootSignature.Get();
        psoDesc.VS = vertexShader.GetBytecode();
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 0;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        D3D12_THROW_IF_FAILED(m_graphicsContext->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipeline.pipelineState)));

        AddGraphicsPipeline(PipelineId::ShadowMap, pipeline);
    }
}