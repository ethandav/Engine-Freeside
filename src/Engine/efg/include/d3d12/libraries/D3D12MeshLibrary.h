#pragma once
#include "..\..\render\types\MeshTypes.h"
#include "..\..\render\types\Handles.h"
#include "..\resources\D3D12GpuMesh.h"
#include "..\resources\D3D12GpuBuffer.h"

#include <vector>

namespace efg::d3d12
{
    class D3D12MeshLibrary
    {
    public:
        Freeside::MeshHandle RegisterMesh(const Freeside::MeshDesc& meshData);
        const GpuMesh& Get(Freeside::MeshHandle handle) const;
        void SetVertexBuffer(Freeside::MeshHandle handle, const GpuBuffer& buffer);
        void SetIndexBuffer(Freeside::MeshHandle handle, const GpuBuffer& buffer);
    private:
        std::vector<GpuMesh> m_meshes;
    };
}