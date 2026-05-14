#pragma once
#include "..\..\render\MeshData.h"
#include "..\resources\D3D12GpuMesh.h"
#include "..\resources\D3D12GpuBuffer.h"

#include <vector>

namespace efg::d3d12
{
    class D3D12MeshLibrary
    {
    public:
        MeshHandle RegisterMesh(const MeshData& meshData);
        const GpuMesh& Get(MeshHandle handle) const;
        void SetVertexBuffer(MeshHandle handle, const GpuBuffer& buffer);
        void SetIndexBuffer(MeshHandle handle, const GpuBuffer& buffer);
    private:
        std::vector<GpuMesh> m_meshes;
    };
}