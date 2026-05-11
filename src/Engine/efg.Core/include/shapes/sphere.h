#pragma once
#include "../../../efg.Renderer/include/MeshData.h"

namespace efg
{
    namespace Shapes
    {
        struct Sphere
        {
            Sphere()
            {
                Build();
            }

            explicit Sphere(float diameterValue) : diameter(diameterValue)
            {
                Build();
            }

            void Build()
            {
                size_t tessellation = 20;

                const size_t verticalSegments = tessellation;
                const size_t horizontalSegments = tessellation * 2;

                vertexCount = static_cast<uint32_t>((verticalSegments + 1) * (horizontalSegments + 1));
                indexCount = static_cast<uint32_t>(verticalSegments * horizontalSegments * 6);

                mesh.vertices.reserve(vertexCount);
                mesh.indices.reserve(indexCount);

                const float radius = diameter / 2.0f;

                for (size_t i = 0; i <= verticalSegments; i++)
                {
                    const float v = 1.0f - float(i) / float(verticalSegments);

                    const float latitude = (float(i) * Math::PI / float(verticalSegments)) - Math::PI_DIV_2;
                    float dy, dxz;

                    Math::ScalarSinCos(&dy, &dxz, latitude);

                    for (size_t j = 0; j <= horizontalSegments; j++)
                    {
                        const float u = float(j) / float(horizontalSegments);

                        const float longitude = float(j) * Math::TwoPI / float(horizontalSegments);
                        float dx, dz;

                        Math::ScalarSinCos(&dx, &dz, longitude);

                        dx *= dxz;
                        dz *= dxz;

                        Vertex vertex;
                        vertex.position.x = dx * radius;
                        vertex.position.y = dy * radius;
                        vertex.position.z = dz * radius;

                        vertex.uv.x = u;
                        vertex.uv.y = v;

                        vertex.normal = Normalize(vertex.position);

                        mesh.vertices.push_back(vertex);
                    }
                }

                const size_t stride = horizontalSegments + 1;

                for (size_t i = 0; i < verticalSegments; i++)
                {
                    for (size_t j = 0; j < horizontalSegments; j++)
                    {
                        const size_t nextI = i + 1;
                        const size_t nextJ = (j + 1) % stride;

                        mesh.indices.push_back(static_cast<uint32_t>(i * stride + j));
                        mesh.indices.push_back(static_cast<uint32_t>(i * stride + nextJ));
                        mesh.indices.push_back(static_cast<uint32_t>(nextI * stride + j));

                        mesh.indices.push_back(static_cast<uint32_t>(i * stride + nextJ));
                        mesh.indices.push_back(static_cast<uint32_t>(nextI * stride + nextJ));
                        mesh.indices.push_back(static_cast<uint32_t>(nextI * stride + j));
                    }
                }
            }

            uint32_t vertexCount = 24;
            uint32_t indexCount = 36;
            float diameter = 1.0f;
            MeshData mesh;
        };
    }
}