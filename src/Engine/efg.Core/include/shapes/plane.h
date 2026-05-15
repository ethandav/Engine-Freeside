#pragma once

#pragma once
#include "..\..\..\efg.Renderer\include\render\types\MeshTypes.h"
#include "..\math\Vec2.h"
#include "..\math\Vec3.h"

#include <cstdint>
#include <vector>


namespace Freeside
{
    namespace Shapes
    {
        struct Plane
        {
            constexpr Plane() = default;

            uint32_t vertexCount = 4;
            uint32_t indexCount = 6;

            MeshData mesh =
            {
                std::vector<Vertex>(vertexCount) = {
                    {Math::Vec3(-0.5f, 0.0f, -0.5f),Math::Vec3(0.0f, 1.0f, 0.0f),Math::Vec2(0.0f, 0.0f)},//Top
                    {Math::Vec3(0.5f, 0.0f, -0.5f),Math::Vec3(0.0f, 1.0f, 0.0f),Math::Vec2(1.0f, 0.0f)},
                    {Math::Vec3(-0.5f, 0.0f, 0.5f),Math::Vec3(0.0f, 1.0f, 0.0f),Math::Vec2(0.0f, 1.0f)},
                    {Math::Vec3(0.5f, 0.0f, 0.5f),Math::Vec3(0.0f, 1.0f, 0.0f),Math::Vec2(1.0f, 1.0f)}
                },
                std::vector<uint32_t>(indexCount) = {
                     2,  1,  0, // Top
                     3,  1,  2
                }
            };
        };
    }
}