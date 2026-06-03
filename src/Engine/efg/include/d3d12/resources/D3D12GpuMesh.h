#pragma once

#include "..\..\render\types\Handles.h"
#include "D3D12GpuBuffer.h"

#include <d3d12.h>
#include <cstdint>

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
        uint64_t vertexBufferSize = 0;
        uint64_t indexBufferSize = 0;

        Freeside::MeshHandle handle = {};
    };
}