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
        struct Wall
        {
            constexpr Wall() = default;

            uint32_t vertexCount = 8;
            uint32_t indexCount = 12;

            MeshData mesh =
            {
                std::vector<Vertex>(vertexCount) = {
                    {Math::Vec3(-0.5f, -0.5f, 0.0f),Math::Vec3(0.0f, 0.0f, -1.0f),Math::Vec2(0.0f, 0.0f)}, // Front
                    {Math::Vec3(0.5f, -0.5f, 0.0f),Math::Vec3(0.0f, 0.0f, -1.0f),Math::Vec2(1.0f, 0.0f)},
                    {Math::Vec3(-0.5f, 0.5f, 0.0f),Math::Vec3(0.0f, 0.0f, -1.0f),Math::Vec2(0.0f, 1.0f)},
                    {Math::Vec3(0.5f, 0.5f, 0.0f),Math::Vec3(0.0f, 0.0f, -1.0f),Math::Vec2(1.0f, 1.0f)},
                    {Math::Vec3(-0.5f, -0.5f, 0.0f),Math::Vec3(0.0f, 0.0f, 1.0f),Math::Vec2(0.0f, 0.0f)}, // Back
                    {Math::Vec3(0.5f, -0.5f, 0.0f),Math::Vec3(0.0f, 0.0f, 1.0f),Math::Vec2(1.0f, 0.0f)},
                    {Math::Vec3(-0.5f, 0.5f, 0.0f),Math::Vec3(0.0f, 0.0f, 1.0f),Math::Vec2(0.0f, 1.0f)},
                    {Math::Vec3(0.5f, 0.5f, 0.0f),Math::Vec3(0.0f, 0.0f, 1.0f),Math::Vec2(1.0f, 1.0f)}
                },
                std::vector<uint32_t>(indexCount) = {
                     2,  1,  0, // Front
                     3,  1,  2,
                     4,  5,  6, // Back
                     6,  5,  7
                }
            };
        };
    }
}