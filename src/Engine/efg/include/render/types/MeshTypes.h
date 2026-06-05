#pragma once

#include "..\..\..\..\Freeside.Core\include\math\Vec3.h"
#include "..\..\..\..\Freeside.Core\include\math\Vec2.h"

#include <vector>

namespace Freeside
{
    struct Vertex
    {
        Freeside::Math::Vec3 position;
        Freeside::Math::Vec3 normal;
        Freeside::Math::Vec2 uv;
        Freeside::Math::Vec3 tangent;
    };

    struct MeshDesc
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    struct Transform
    {
        Freeside::Math::Vec3 position = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
        Freeside::Math::Vec3 rotation = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
        Freeside::Math::Vec3 scale = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
    };
}