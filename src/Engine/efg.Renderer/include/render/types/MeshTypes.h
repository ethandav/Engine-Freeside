#pragma once

#include "..\..\..\..\efg.Core\include\math\Vec3.h"
#include "..\..\..\..\efg.Core\include\math\Vec2.h"

#include <vector>

namespace Freeside
{
    struct Vertex
    {
        Freeside::Math::Vec3 position;
        Freeside::Math::Vec3 normal;
        Freeside::Math::Vec2 uv;
    };

    struct MeshData
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
}