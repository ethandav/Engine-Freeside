#pragma once

#include <cstdint>
#include <vector>

#include "..\..\efg.Core\include\math\Vec2.h"
#include "..\..\efg.Core\include\math\Vec3.h"
#include "..\..\efg.Core\include\math\Vec4.h"

namespace efg
{
    struct Vertex
    {
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
    };

    struct MeshData
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    struct MeshHandle
    {
        uint32_t index = UINT32_MAX;

        bool IsValid() const
        {
            return index != UINT32_MAX;
        }
    };
}