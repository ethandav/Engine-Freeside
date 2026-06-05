#include "..\..\..\..\include\d3d12\factories\D3D12PipelineFactory.h"
#include "..\..\..\..\include\d3d12\factories\D3D12RootSignatureFactory.h"
#include "..\..\..\..\include\d3d12\libraries\D3D12ShaderLibrary.h"
#include "..\..\..\..\include\d3d12\types\D3D12PipelineTypes.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

namespace efg::d3d12::ForwardLitGeometryPipeline
{
    GraphicsPipelineState CreatePipeline(const D3D12PipelineFactory& pipelineFactory, const D3D12RootSignatureFactory& rootSignatureFactory, const D3D12ShaderLibrary& shaderLibrary)
    {
        GraphicsPipelineState pipeline = {};

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

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
        materialTextures.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 5);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::BaseColorTexture)].InitAsDescriptorTable(1, &materialTextures, D3D12_SHADER_VISIBILITY_PIXEL);

        rootParameters[static_cast<UINT>(ForwardLitRootParameter::DirectionalLightsSrv)].InitAsShaderResourceView(2, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::DirectionalShadowDataSrv)].InitAsShaderResourceView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[static_cast<UINT>(ForwardLitRootParameter::PointShadowDataSrv)].InitAsShaderResourceView(4, 0, D3D12_SHADER_VISIBILITY_PIXEL);

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

        pipeline.rootSignature = rootSignatureFactory.CreateRootSignature(rootSignatureDesc);

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;

        pipeline.pipelineState = pipelineFactory.CreateGraphicsPipeline(psoDesc);

        return pipeline;
    }
}