#pragma once
#include "..\..\..\efg\include\render\types\MeshTypes.h"
#include "Vec3.h"
#include "Vec2.h"
#include <vector>
#include <cstdint>

namespace Freeside::Math
{
    inline void GenerateTangents(std::vector<Freeside::Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        std::vector<Freeside::Math::Vec3> accumulatedTangents(vertices.size(), Freeside::Math::Vec3(0.0f, 0.0f, 0.0f));
        std::vector<Freeside::Math::Vec3> accumulatedBitangents(vertices.size(), Freeside::Math::Vec3(0.0f, 0.0f, 0.0f));

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            uint32_t i0 = indices[i + 0];
            uint32_t i1 = indices[i + 1];
            uint32_t i2 = indices[i + 2];

            Freeside::Vertex& v0 = vertices[i0];
            Freeside::Vertex& v1 = vertices[i1];
            Freeside::Vertex& v2 = vertices[i2];

            const Freeside::Math::Vec3& p0 = v0.position;
            const Freeside::Math::Vec3& p1 = v1.position;
            const Freeside::Math::Vec3& p2 = v2.position;

            const Freeside::Math::Vec2& uv0 = v0.uv;
            const Freeside::Math::Vec2& uv1 = v1.uv;
            const Freeside::Math::Vec2& uv2 = v2.uv;

            Freeside::Math::Vec3 edge1 = p1 - p0;
            Freeside::Math::Vec3 edge2 = p2 - p0;

            Freeside::Math::Vec2 deltaUV1 = uv1 - uv0;
            Freeside::Math::Vec2 deltaUV2 = uv2 - uv0;

            float denominator = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;

            if (std::abs(denominator) < 1e-8f)
            {
                continue;
            }

            float r = 1.0f / denominator;

            Freeside::Math::Vec3 tangent =
            {
                r * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                r * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                r * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
            };

            Freeside::Math::Vec3 bitangent =
            {
                r * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                r * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                r * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
            };

            accumulatedTangents[i0] += tangent;
            accumulatedTangents[i1] += tangent;
            accumulatedTangents[i2] += tangent;

            accumulatedBitangents[i0] += bitangent;
            accumulatedBitangents[i1] += bitangent;
            accumulatedBitangents[i2] += bitangent;
        }

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            Freeside::Vertex& vertex = vertices[i];

            Freeside::Math::Vec3 n = Normalize(vertex.normal);
            Freeside::Math::Vec3 t = accumulatedTangents[i];
            Freeside::Math::Vec3 b = accumulatedBitangents[i];

            // Gram-Schmidt: remove normal component from tangent.
            t = t - n * Dot(n, t);

            if (LengthSquared(t) < 1e-8f)
            {
                t = std::abs(n.y) < 0.999f
                    ? Normalize(Cross(Freeside::Math::Vec3(0.0f, 1.0f, 0.0f), n))
                    : Normalize(Cross(Freeside::Math::Vec3(1.0f, 0.0f, 0.0f), n));

                // Pick a fallback bitangent consistent with the fallback tangent.
                b = Cross(n, t);
            }
            else
            {
                t = Normalize(t);
            }

            // Handedness/sign.
            //
            // If accumulated bitangent points opposite of cross(N, T),
            // tangent.w should be -1. Otherwise +1.
            float handedness = Dot(Cross(n, t), b) < 0.0f ? -1.0f : 1.0f;

            vertex.tangent = Freeside::Math::Vec4(t.x, t.y, t.z, handedness);
        }
    }
}
