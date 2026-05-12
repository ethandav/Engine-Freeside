#pragma once
#include <cstdint>
#include <vector>
#include "..\..\..\efg.Renderer\include\render\MeshData.h"

namespace efg
{
    namespace Shapes
    {
        struct Cube
        {
            constexpr Cube() = default;

            uint32_t vertexCount = 24;
            uint32_t indexCount = 36;

            MeshData mesh =
            {
                std::vector<efg::Vertex>(vertexCount) = {
                    {Vec3(-0.5f, 0.5f, -0.5f),Vec3(0.0f, 1.0f, 0.0f),Vec2(0.0f, 0.0f)},//Top
                    {Vec3(0.5f, 0.5f, -0.5f),Vec3(0.0f, 1.0f, 0.0f),Vec2(1.0f, 0.0f)},
                    {Vec3(-0.5f, 0.5f, 0.5f),Vec3(0.0f, 1.0f, 0.0f),Vec2(0.0f, 1.0f)},
                    {Vec3(0.5f, 0.5f, 0.5f),Vec3(0.0f, 1.0f, 0.0f),Vec2(1.0f, 1.0f)},
                    {Vec3(-0.5f, 0.5f, 0.5f),Vec3(0.0f, 0.0f, 1.0f),Vec2(0.0f, 0.0f)}, //Front
                    {Vec3(0.5f, 0.5f, 0.5f),Vec3(0.0f, 0.0f, 1.0f),Vec2(1.0f, 0.0f)},
                    {Vec3(-0.5f, -0.5f, 0.5f),Vec3(0.0f, 0.0f, 1.0f),Vec2(0.0f, 1.0f)},
                    {Vec3(0.5f, -0.5f, 0.5f),Vec3(0.0f, 0.0f, 1.0f),Vec2(1.0f, 1.0f)},
                    {Vec3(0.5f, 0.5f, 0.5f),Vec3(1.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)}, //Right
                    {Vec3(0.5f, 0.5f, -0.5f),Vec3(1.0f, 0.0f, 0.0f),Vec2(1.0f, 0.0f)},
                    {Vec3(0.5f, -0.5f, 0.5f),Vec3(1.0f, 0.0f, 0.0f),Vec2(0.0f, 1.0f)},
                    {Vec3(0.5f, -0.5f, -0.5f),Vec3(1.0f, 0.0f, 0.0f),Vec2(1.0f, 1.0f)},
                    {Vec3(0.5f, 0.5f, -0.5f),Vec3(0.0f, 0.0f, -1.0f),Vec2(0.0f, 0.0f)}, //Back
                    {Vec3(-0.5f, 0.5f, -0.5f),Vec3(0.0f, 0.0f, -1.0f),Vec2(1.0f, 0.0f)},
                    {Vec3(0.5f, -0.5f, -0.5f),Vec3(0.0f, 0.0f, -1.0f),Vec2(0.0f, 1.0f)},
                    {Vec3(-0.5f, -0.5f, -0.5f),Vec3(0.0f, 0.0f, -1.0f),Vec2(1.0f, 1.0f)},
                    {Vec3(-0.5f, 0.5f, -0.5f),Vec3(-1.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)}, //Left
                    {Vec3(-0.5f, 0.5f, 0.5f),Vec3(-1.0f, 0.0f, 0.0f),Vec2(1.0f, 0.0f)},
                    {Vec3(-0.5f, -0.5f, -0.5f),Vec3(-1.0f, 0.0f, 0.0f),Vec2(0.0f, 1.0f)},
                    {Vec3(-0.5f, -0.5f, 0.5f),Vec3(-1.0f, 0.0f, 0.0f),Vec2(1.0f, 1.0f)},
                    {Vec3(-0.5f, -0.5f, 0.5f),Vec3(0.0f, -1.0f, 0.0f),Vec2(0.0f, 0.0f)}, //Bottom
                    {Vec3(0.5f, -0.5f, 0.5f),Vec3(0.0f, -1.0f, 0.0f),Vec2(1.0f, 0.0f)},
                    {Vec3(-0.5f, -0.5f, -0.5f),Vec3(0.0f, -1.0f, 0.0f),Vec2(0.0f, 1.0f)},
                    {Vec3(0.5f, -0.5f, -0.5f),Vec3(0.0f, -1.0f, 0.0f),Vec2(1.0f, 1.0f)}
                },
                std::vector<uint32_t>(indexCount) = {
                     2,  1,  0, // Top
                     3,  1,  2,
                     6,  5,  4, // Back
                     7,  5,  6,
                    10,  9,  8, // Right
                    11,  9, 10,
                    14, 13, 12, // Front
                    15, 13, 14,
                    18, 17, 16, // Left
                    19, 17, 18,
                    22, 21, 20, // Bottom
                    23, 21, 22
                }
            };
        };
    }
}