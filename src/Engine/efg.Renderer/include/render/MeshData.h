#pragma once

#include <cstdint>
#include <vector>

#include "..\..\..\efg.Core\include\math\Vec2.h"
#include "..\..\..\efg.Core\include\math\Vec3.h"
#include "..\..\..\efg.Core\include\math\Vec4.h"

namespace efg
{
    struct Vertex
    {
        Math::Vec3 position;
        Math::Vec3 normal;
        Math::Vec2 uv;
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

        bool operator==(const MeshHandle& other) const
        {
            return index == other.index;
        }

        bool operator!=(const MeshHandle& other) const
        {
            return !(*this == other);
        }
    };
}