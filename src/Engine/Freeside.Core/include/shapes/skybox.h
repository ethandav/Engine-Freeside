#pragma once

#include "..\..\..\efg\include\render\types\MeshTypes.h"
#include "..\math\Vec3.h"

#include <cstdint>
#include <vector>

namespace Freeside
{
    namespace Shapes
    {
        struct SkyboxCube
        {
            SkyboxCube()
                : mesh(CreateMesh())
            {
            }

            uint32_t vertexCount = 8;
            uint32_t indexCount = 36;

            MeshDesc mesh;

        private:
            static MeshDesc CreateMesh()
            {
                MeshDesc mesh = {};

                mesh.vertices =
                {
                    // Back face corners
                    { Math::Vec3(-1.0f, -1.0f, -1.0f) }, // 0
                    { Math::Vec3(-1.0f,  1.0f, -1.0f) }, // 1
                    { Math::Vec3(1.0f,  1.0f, -1.0f) }, // 2
                    { Math::Vec3(1.0f, -1.0f, -1.0f) }, // 3

                    // Front face corners
                    { Math::Vec3(-1.0f, -1.0f,  1.0f) }, // 4
                    { Math::Vec3(-1.0f,  1.0f,  1.0f) }, // 5
                    { Math::Vec3(1.0f,  1.0f,  1.0f) }, // 6
                    { Math::Vec3(1.0f, -1.0f,  1.0f) }, // 7
                };

                // Winding intended for viewing the cube from the inside.
                mesh.indices =
                {
                    // +Z / front
                    4, 6, 5,
                    4, 7, 6,

                    // -Z / back
                    0, 1, 2,
                    0, 2, 3,

                    // +X / right
                    3, 2, 6,
                    3, 6, 7,

                    // -X / left
                    0, 5, 1,
                    0, 4, 5,

                    // +Y / top
                    1, 5, 6,
                    1, 6, 2,

                    // -Y / bottom
                    0, 3, 7,
                    0, 7, 4
                };

                return mesh;
            }
        };
    }
}