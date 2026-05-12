#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <vector>

#include "..\render\MeshData.h"
#include "D3D12BufferData.h"

using Microsoft::WRL::ComPtr;

namespace efg::d3d12
{
    struct GpuMesh
    {
        GpuBuffer vertexBuffer;
        GpuBuffer indexBuffer;

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
        D3D12_INDEX_BUFFER_VIEW indexBufferView = {};

        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        UINT64 vertexBufferSize = 0;
        UINT64 indexBufferSize = 0;

        MeshHandle handle = {};
    };

    class D3D12MeshLibrary
    {
    public:
        void InitializeBuiltIns(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
        MeshHandle RegisterMesh(const MeshData& meshData);
        const GpuMesh& Get(MeshHandle handle) const;
        void SetVertexBuffer(MeshHandle handle, const GpuBuffer& buffer);
        void SetIndexBuffer(MeshHandle handle, const GpuBuffer& buffer);
    private:
        std::vector<GpuMesh> m_meshes;
    };
}