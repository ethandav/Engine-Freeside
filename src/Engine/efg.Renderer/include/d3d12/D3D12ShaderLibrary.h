#pragma once
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <stdexcept>
#include <array>

using Microsoft::WRL::ComPtr;

enum class ShaderId : uint32_t
{
    TriangleVS,
    TrianglePS,

    Count
};

struct ShaderBytecode
{
    ComPtr<ID3DBlob> blob;

    D3D12_SHADER_BYTECODE GetD3D12Bytecode() const
    {
        D3D12_SHADER_BYTECODE bytecode = {};
        bytecode.pShaderBytecode = blob->GetBufferPointer();
        bytecode.BytecodeLength = blob->GetBufferSize();
        return bytecode;
    }
};

class D3D12ShaderLibrary
{
public:
	void Initialize();
    const ShaderBytecode& Get(ShaderId id) const;
private:
    void CompileAllShaders();
	ComPtr<ID3DBlob> CompileShaderFromFile(const std::wstring& filePath, const std::string& entryPoint, const std::string& target);
    void AddShader(ShaderId id, const std::wstring& filePath, const std::string& entryPoint, const std::string& target);

    std::array<ShaderBytecode, static_cast<size_t>(ShaderId::Count)> m_shaders;
};