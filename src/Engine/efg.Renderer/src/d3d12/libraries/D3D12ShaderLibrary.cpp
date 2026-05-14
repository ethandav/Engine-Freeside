#include "..\..\..\include\d3d12\libraries\D3D12ShaderLibrary.h"

#include <stdexcept>

namespace efg::d3d12
{
    void D3D12ShaderLibrary::Initialize()
    {
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
            ShaderId::TriangleVS,
            L"..\\Engine\\efg.Renderer\\assets\\shaders\\triangle.hlsl",
            "VSMain",
            "vs_5_0"
        );

        AddShader(
            ShaderId::TrianglePS,
            L"..\\Engine\\efg.Renderer\\assets\\shaders\\triangle.hlsl",
            "PSMain",
            "ps_5_0"
        );

        AddShader(
            ShaderId::GeometryVS,
            L"..\\Engine\\efg.Renderer\\assets\\shaders\\geometry.hlsl",
            "VSMain",
            "vs_5_0"
        );

        AddShader(
            ShaderId::GeometryPS,
            L"..\\Engine\\efg.Renderer\\assets\\shaders\\geometry.hlsl",
            "PSMain",
            "ps_5_0"
        );
    }

    Microsoft::WRL::ComPtr<ID3DBlob> D3D12ShaderLibrary::CompileShaderFromFile(const std::wstring& filePath, const std::string& entryPoint, const std::string& target)
    {
        UINT compileFlags = 0;

#if defined(_DEBUG)
        compileFlags |= D3DCOMPILE_DEBUG;
        compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3DCompileFromFile(
            filePath.c_str(),
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint.c_str(),
            target.c_str(),
            compileFlags,
            0,
            shaderBlob.GetAddressOf(),
            errorBlob.GetAddressOf()
        );

        if (FAILED(hr))
        {
            if (errorBlob)
            {
                OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
            }

            throw std::runtime_error("Failed to compile shader.");
        }

        return shaderBlob;
    }

    void D3D12ShaderLibrary::AddShader(
        ShaderId id,
        const std::wstring& filePath,
        const std::string& entryPoint,
        const std::string& target)
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