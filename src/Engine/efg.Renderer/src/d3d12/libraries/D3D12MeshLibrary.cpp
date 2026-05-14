#include "..\..\..\include\d3d12\libraries\D3D12MeshLibrary.h"

#include <stdexcept>

namespace efg::d3d12
{
    MeshHandle D3D12MeshLibrary::RegisterMesh(const MeshData& meshData)
    {
        if (meshData.vertices.empty())
        {
            throw std::runtime_error("Cannot create mesh with no vertices.");
        }

        GpuMesh gpuMesh = {};
        gpuMesh.vertexCount = static_cast<uint32_t>(meshData.vertices.size());
        gpuMesh.indexCount = static_cast<uint32_t>(meshData.indices.size());
        gpuMesh.vertexBufferSize = meshData.vertices.size() * sizeof(Vertex);
        gpuMesh.indexBufferSize = meshData.indices.size() * sizeof(uint32_t);
        gpuMesh.vertexBufferView.StrideInBytes = sizeof(Vertex);
        gpuMesh.vertexBufferView.SizeInBytes = static_cast<UINT>(gpuMesh.vertexBufferSize);
        gpuMesh.indexBufferView.Format = DXGI_FORMAT_R32_UINT;
        gpuMesh.indexBufferView.SizeInBytes = static_cast<UINT>(gpuMesh.indexBufferSize);
        gpuMesh.handle.index = static_cast<uint32_t>(m_meshes.size());

        m_meshes.push_back(std::move(gpuMesh));

        return MeshHandle
        {
            static_cast<uint32_t>(m_meshes.size() - 1)
        };
    }

    const GpuMesh& D3D12MeshLibrary::Get(MeshHandle handle) const
    {
        if (!handle.IsValid() || handle.index >= m_meshes.size())
        {
            throw std::runtime_error("Invalid mesh handle.");
        }

        return m_meshes[handle.index];
    }

    void D3D12MeshLibrary::SetVertexBuffer(MeshHandle handle, const GpuBuffer& buffer)
    {
        m_meshes[handle.index].vertexBufferView.BufferLocation = buffer.resource->GetGPUVirtualAddress();
        m_meshes[handle.index].vertexBuffer.resource = buffer.resource;
    }

    void D3D12MeshLibrary::SetIndexBuffer(MeshHandle handle, const GpuBuffer& buffer)
    {
        m_meshes[handle.index].indexBufferView.BufferLocation = buffer.resource->GetGPUVirtualAddress();
        m_meshes[handle.index].indexBuffer.resource = buffer.resource;
    }
}