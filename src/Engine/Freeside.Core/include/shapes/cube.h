#pragma once
#include "..\..\..\efg\include\render\types\MeshTypes.h"
#include "..\math\Vec2.h"
#include "..\math\Vec3.h"
#include "..\math\Meshes.h"

#include <cstdint>
#include <vector>


namespace Freeside
{
    namespace Shapes
    {
        struct Cube
        {
            Cube()
                : mesh(CreateMesh())
            {
            }

            uint32_t vertexCount = 24;
            uint32_t indexCount = 36;

            MeshDesc mesh;

        private:
            static MeshDesc CreateMesh()
            {
                MeshDesc mesh =
                {
                    {
                        {Math::Vec3(-0.5f, 0.5f, -0.5f),Math::Vec3(0.0f, 1.0f, 0.0f),Math::Vec2(0.0f, 0.0f)},//Top
                        {Math::Vec3(0.5f, 0.5f, -0.5f),Math::Vec3(0.0f, 1.0f, 0.0f),Math::Vec2(1.0f, 0.0f)},
                        {Math::Vec3(-0.5f, 0.5f, 0.5f),Math::Vec3(0.0f, 1.0f, 0.0f),Math::Vec2(0.0f, 1.0f)},
                        {Math::Vec3(0.5f, 0.5f, 0.5f),Math::Vec3(0.0f, 1.0f, 0.0f),Math::Vec2(1.0f, 1.0f)},
                        {Math::Vec3(-0.5f, 0.5f, 0.5f),Math::Vec3(0.0f, 0.0f, 1.0f),Math::Vec2(0.0f, 0.0f)}, //Front
                        {Math::Vec3(0.5f, 0.5f, 0.5f),Math::Vec3(0.0f, 0.0f, 1.0f),Math::Vec2(1.0f, 0.0f)},
                        {Math::Vec3(-0.5f, -0.5f, 0.5f),Math::Vec3(0.0f, 0.0f, 1.0f),Math::Vec2(0.0f, 1.0f)},
                        {Math::Vec3(0.5f, -0.5f, 0.5f),Math::Vec3(0.0f, 0.0f, 1.0f),Math::Vec2(1.0f, 1.0f)},
                        {Math::Vec3(0.5f, 0.5f, 0.5f),Math::Vec3(1.0f, 0.0f, 0.0f),Math::Vec2(0.0f, 0.0f)}, //Right
                        {Math::Vec3(0.5f, 0.5f, -0.5f),Math::Vec3(1.0f, 0.0f, 0.0f),Math::Vec2(1.0f, 0.0f)},
                        {Math::Vec3(0.5f, -0.5f, 0.5f),Math::Vec3(1.0f, 0.0f, 0.0f),Math::Vec2(0.0f, 1.0f)},
                        {Math::Vec3(0.5f, -0.5f, -0.5f),Math::Vec3(1.0f, 0.0f, 0.0f),Math::Vec2(1.0f, 1.0f)},
                        {Math::Vec3(0.5f, 0.5f, -0.5f),Math::Vec3(0.0f, 0.0f, -1.0f),Math::Vec2(0.0f, 0.0f)}, //Back
                        {Math::Vec3(-0.5f, 0.5f, -0.5f),Math::Vec3(0.0f, 0.0f, -1.0f),Math::Vec2(1.0f, 0.0f)},
                        {Math::Vec3(0.5f, -0.5f, -0.5f),Math::Vec3(0.0f, 0.0f, -1.0f),Math::Vec2(0.0f, 1.0f)},
                        {Math::Vec3(-0.5f, -0.5f, -0.5f),Math::Vec3(0.0f, 0.0f, -1.0f),Math::Vec2(1.0f, 1.0f)},
                        {Math::Vec3(-0.5f, 0.5f, -0.5f),Math::Vec3(-1.0f, 0.0f, 0.0f),Math::Vec2(0.0f, 0.0f)}, //Left
                        {Math::Vec3(-0.5f, 0.5f, 0.5f),Math::Vec3(-1.0f, 0.0f, 0.0f),Math::Vec2(1.0f, 0.0f)},
                        {Math::Vec3(-0.5f, -0.5f, -0.5f),Math::Vec3(-1.0f, 0.0f, 0.0f),Math::Vec2(0.0f, 1.0f)},
                        {Math::Vec3(-0.5f, -0.5f, 0.5f),Math::Vec3(-1.0f, 0.0f, 0.0f),Math::Vec2(1.0f, 1.0f)},
                        {Math::Vec3(-0.5f, -0.5f, 0.5f),Math::Vec3(0.0f, -1.0f, 0.0f),Math::Vec2(0.0f, 0.0f)}, //Bottom
                        {Math::Vec3(0.5f, -0.5f, 0.5f),Math::Vec3(0.0f, -1.0f, 0.0f),Math::Vec2(1.0f, 0.0f)},
                        {Math::Vec3(-0.5f, -0.5f, -0.5f),Math::Vec3(0.0f, -1.0f, 0.0f),Math::Vec2(0.0f, 1.0f)},
                        {Math::Vec3(0.5f, -0.5f, -0.5f),Math::Vec3(0.0f, -1.0f, 0.0f),Math::Vec2(1.0f, 1.0f)}
                    },
                    {
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

                Math::GenerateTangents(mesh.vertices, mesh.indices);

                return mesh;
            }
        };
    }
}