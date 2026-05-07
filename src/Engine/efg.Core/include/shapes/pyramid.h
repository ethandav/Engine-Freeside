#pragma once

namespace efg
{
    namespace Shapes
    {
        struct Pyramid
        {
            constexpr Pyramid() = default;

            uint32_t vertexCount = 16;
            uint32_t indexCount = 18;

            MeshData mesh =
            {
                std::vector<efg::Vertex>(vertexCount) = {
                    {Vec3(0.5f, -0.5f, -0.5f) ,Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 0.0f)},
                    {Vec3(0.0f, 0.5f, 0.0f)   ,Vec3(0.0f, 0.0f, -1.0f),Vec2(0.5f, 1.0f) },
                    {Vec3(-0.5f, -0.5f, -0.5f),Vec3(0.0f, 0.0f, -1.0f),Vec2(0.0f, 0.0f) },
                    {Vec3(0.5f, -0.5f, 0.5f)  ,Vec3(0.0f, 0.0f, 1.0f) ,Vec2(1.0f, 0.0f) },
                    {Vec3(0.0f, 0.5f, 0.0f)   ,Vec3(0.0f, 0.0f, 1.0f) ,Vec2(0.5f, 1.0f) },
                    {Vec3(-0.5f, -0.5f, 0.5f) ,Vec3(0.0f, 0.0f, 1.0f) ,Vec2(0.0f, 0.0f) },
                    {Vec3(-0.5f, -0.5f, -0.5f),Vec3(-1.0f, 0.0f, 0.0f),Vec2(1.0f, 0.0f) },
                    {Vec3(0.0f, 0.5f, 0.0f)   ,Vec3(-1.0f, 0.0f, 0.0f),Vec2(0.5f, 1.0f) },
                    {Vec3(-0.5f, -0.5f, 0.5f) ,Vec3(-1.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f) },
                    {Vec3(0.5f, -0.5f, -0.5f) ,Vec3(1.0f, 0.0f, 0.0f) ,Vec2(1.0f, 0.0f) },
                    {Vec3(0.0f, 0.5f, 0.0f)   ,Vec3(1.0f, 0.0f, 0.0f) ,Vec2(0.5f, 1.0f) },
                    {Vec3(0.5f, -0.5f, 0.5f)  ,Vec3(1.0f, 0.0f, 0.0f) ,Vec2(0.0f, 0.0f) },
                    {Vec3(0.5f, -0.5f, 0.5f)  ,Vec3(0.0f, -1.0f, 0.0f),Vec2(0.0f, 0.0f) },
                    {Vec3(0.5f, -0.5f, -0.5f) ,Vec3(0.0f, -1.0f, 0.0f),Vec2(1.0f, 0.0f) },
                    {Vec3(-0.5f, -0.5f, -0.5f),Vec3(0.0f, -1.0f, 0.0f),Vec2(0.0f, 1.0f) },
                    {Vec3(-0.5f, -0.5f, 0.5f) ,Vec3(0.0f, -1.0f, 0.0f),Vec2(1.0f, 1.0f) }
                },
                std::vector<uint32_t>(indexCount) = {
                    2, 1, 0,
                    5, 3, 4,
                    8, 7, 6,
                    11, 9, 10,
                    14, 13, 15,
                    15, 13, 12
                }
            };
        };
    }
}