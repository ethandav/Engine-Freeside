#include "..\..\..\include\d3d12\libraries\D3D12ShaderLibrary.h"

#include <stdexcept>
#include <vector>

#pragma comment(lib, "dxcompiler.lib")

namespace efg::d3d12
{
    static void ThrowIfFailed(HRESULT hr, const char* message)
    {
        if (FAILED(hr))
        {
            throw std::runtime_error(message);
        }
    }

    void D3D12ShaderLibrary::Initialize()
    {
        ThrowIfFailed(
            DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_dxcUtils)),
            "Failed to create DXC utils."
        );

        ThrowIfFailed(
            DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_dxcCompiler)),
            "Failed to create DXC compiler."
        );

        ThrowIfFailed(
            m_dxcUtils->CreateDefaultIncludeHandler(&m_dxcIncludeHandler),
            "Failed to create DXC include handler."
        );

        CompileAllShaders();
    }

    const ShaderBytecode& D3D12ShaderLibrary::Get(ShaderId id) const
    {
        const size_t index = static_cast<size_t>(id);

        if (index >= m_shaders.size())
        {
            throw std::runtime_error("Invalid shader id.");
        }

        const ShaderBytecode& shader = m_shaders[index];

        if (!shader.blob)
        {
            throw std::runtime_error("Shader was not compiled.");
        }

        return shader;
    }

    void D3D12ShaderLibrary::CompileAllShaders()
    {
        AddShader(
            ShaderId::GeometryVS,
            L"..\\Engine\\efg.Renderer\\assets\\shaders\\GeometryTransform_VS.hlsl",
            L"VSMain",
            L"vs_6_0"
        );

        AddShader(
            ShaderId::ForwardLightingPS,
            L"..\\Engine\\efg.Renderer\\assets\\shaders\\ForwardLighting_PS.hlsl",
            L"PSMain",
            L"ps_6_0"
        );
    }

    Microsoft::WRL::ComPtr<IDxcBlob> D3D12ShaderLibrary::CompileShaderFromFile(
        const std::wstring& filePath,
        const std::wstring& entryPoint,
        const std::wstring& target)
    {
        Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;

        ThrowIfFailed(
            m_dxcUtils->LoadFile(
                filePath.c_str(),
                nullptr,
                &sourceBlob),
            "Failed to load shader file."
        );

        DxcBuffer sourceBuffer = {};
        sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
        sourceBuffer.Size = sourceBlob->GetBufferSize();
        sourceBuffer.Encoding = DXC_CP_UTF8;

        std::vector<LPCWSTR> arguments;

        arguments.push_back(filePath.c_str());

        arguments.push_back(L"-E");
        arguments.push_back(entryPoint.c_str());

        arguments.push_back(L"-T");
        arguments.push_back(target.c_str());

        arguments.push_back(L"-HV");
        arguments.push_back(L"2021");

#if defined(_DEBUG)
        arguments.push_back(L"-Zi");
        arguments.push_back(L"-Od");
        arguments.push_back(L"-Qembed_debug");
#else
        arguments.push_back(L"-O3");
#endif

        Microsoft::WRL::ComPtr<IDxcResult> result;

        ThrowIfFailed(
            m_dxcCompiler->Compile(
                &sourceBuffer,
                arguments.data(),
                static_cast<UINT32>(arguments.size()),
                m_dxcIncludeHandler.Get(),
                IID_PPV_ARGS(&result)),
            "DXC Compile call failed."
        );

        Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors;
        result->GetOutput(
            DXC_OUT_ERRORS,
            IID_PPV_ARGS(&errors),
            nullptr);

        if (errors && errors->GetStringLength() > 0)
        {
            OutputDebugStringA(errors->GetStringPointer());
        }

        HRESULT status = S_OK;
        ThrowIfFailed(
            result->GetStatus(&status),
            "Failed to get DXC compile status."
        );

        if (FAILED(status))
        {
            throw std::runtime_error("DXC shader compilation failed. Check debug output.");
        }

        Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob;

        ThrowIfFailed(
            result->GetOutput(
                DXC_OUT_OBJECT,
                IID_PPV_ARGS(&shaderBlob),
                nullptr),
            "Failed to get DXC shader object."
        );

        if (!shaderBlob)
        {
            throw std::runtime_error("DXC returned null shader blob.");
        }

        return shaderBlob;
    }

    void D3D12ShaderLibrary::AddShader(
        ShaderId id,
        const std::wstring& filePath,
        const std::wstring& entryPoint,
        const std::wstring& target)
    {
        const size_t index = static_cast<size_t>(id);

        if (index >= m_shaders.size())
        {
            throw std::runtime_error("Invalid shader id.");
        }

        ShaderBytecode shader = {};
        shader.blob = CompileShaderFromFile(filePath, entryPoint, target);

        m_shaders[index] = std::move(shader);
    }
}