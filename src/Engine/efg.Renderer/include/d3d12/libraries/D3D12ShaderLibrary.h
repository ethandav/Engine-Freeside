#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <Windows.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>

namespace efg::d3d12
{
    enum class ShaderId : uint32_t
    {
        ShadowMapVS,

        GeometryVS,
        GeometryPS,

        Count
    };

    struct ShaderBytecode
    {
        Microsoft::WRL::ComPtr<IDxcBlob> blob = nullptr;

        D3D12_SHADER_BYTECODE GetBytecode() const
        {
            return
            {
                blob ? blob->GetBufferPointer() : nullptr,
                blob ? blob->GetBufferSize() : 0
            };
        }
    };

    class D3D12ShaderLibrary
    {
    public:
        void Initialize();

        const ShaderBytecode& Get(ShaderId id) const;

    private:
        void CompileAllShaders();

        Microsoft::WRL::ComPtr<IDxcBlob> CompileShaderFromFile(
            const std::wstring& filePath,
            const std::wstring& entryPoint,
            const std::wstring& target);

        void AddShader(
            ShaderId id,
            const std::wstring& filePath,
            const std::wstring& entryPoint,
            const std::wstring& target);

    private:
        std::array<ShaderBytecode, static_cast<size_t>(ShaderId::Count)> m_shaders = {};

        Microsoft::WRL::ComPtr<IDxcUtils> m_dxcUtils;
        Microsoft::WRL::ComPtr<IDxcCompiler3> m_dxcCompiler;
        Microsoft::WRL::ComPtr<IDxcIncludeHandler> m_dxcIncludeHandler;
    };
}