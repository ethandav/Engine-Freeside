#include "..\..\..\..\include\d3d12\factories\D3D12PipelineFactory.h"
#include "..\..\..\..\include\d3d12\factories\D3D12RootSignatureFactory.h"
#include "..\..\..\..\include\d3d12\libraries\D3D12ShaderLibrary.h"
#include "..\..\..\..\include\d3d12\types\D3D12PipelineTypes.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

namespace efg::d3d12::TonemapPipeline
{
    GraphicsPipelineState CreatePipeline(const D3D12PipelineFactory& pipelineFactory, const D3D12RootSignatureFactory& rootSignatureFactory, const D3D12ShaderLibrary& shaderLibrary)
    {
        GraphicsPipelineState pipeline = {};

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

        CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
            0, // s0
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP
        );

        std::array<CD3DX12_ROOT_PARAMETER, 1> rootParameters = {};

        CD3DX12_DESCRIPTOR_RANGE sceneColorRange = {};
        sceneColorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        rootParameters[0].InitAsDescriptorTable(1, &sceneColorRange, D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC staticSamplers[] =
        {
            linearClampSampler,
        };

        D3D12_ROOT_SIGNATURE_FLAGS flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

        rootSignatureDesc.Init(
            static_cast<UINT>(rootParameters.size()),
            rootParameters.data(),
            1,
            &linearClampSampler,
            flags
        );

        pipeline.rootSignature = rootSignatureFactory.CreateRootSignature(rootSignatureDesc);

        D3D12_INPUT_LAYOUT_DESC inputLayout = {};
        inputLayout.pInputElementDescs = nullptr;
        inputLayout.NumElements = 0;

        const ShaderBytecode& vertexShader = shaderLibrary.Get(ShaderId::TonemapVS);
        const ShaderBytecode& pixelShader = shaderLibrary.Get(ShaderId::TonemapPS);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = inputLayout;
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
        psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
        psoDesc.SampleDesc.Count = 1;

        pipeline.pipelineState = pipelineFactory.CreateGraphicsPipeline(psoDesc);

        return pipeline;
    }
}