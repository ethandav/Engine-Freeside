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
        // Clear existing tangents.
        for (Freeside::Vertex& vertex : vertices)
        {
            vertex.tangent = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
        }

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            Freeside::Vertex& v0 = vertices[indices[i + 0]];
            Freeside::Vertex& v1 = vertices[indices[i + 1]];
            Freeside::Vertex& v2 = vertices[indices[i + 2]];

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

            float denominator =
                deltaUV1.x * deltaUV2.y -
                deltaUV2.x * deltaUV1.y;

            if (std::abs(denominator) < 1e-8f)
            {
                // Degenerate UV mapping. Skip this triangle.
                continue;
            }

            float r = 1.0f / denominator;

            Freeside::Math::Vec3 tangent =
            {
                r * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                r * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                r * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
            };

            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;
        }

        // Normalize and orthogonalize tangents.
        for (Freeside::Vertex& vertex : vertices)
        {
            Freeside::Math::Vec3 n = Normalize(vertex.normal);
            Freeside::Math::Vec3 t = vertex.tangent;

            // Gram-Schmidt: remove any component of tangent pointing along normal.
            t = t - n * Dot(n, t);

            if (LengthSquared(t) < 1e-8f)
            {
                // Fallback tangent if UVs were bad or missing.
                t = std::abs(n.y) < 0.999f
                    ? Normalize(Cross(Freeside::Math::Vec3(0.0f, 1.0f, 0.0f), n))
                    : Normalize(Cross(Freeside::Math::Vec3(1.0f, 0.0f, 0.0f), n));
            }
            else
            {
                t = Normalize(t);
            }

            vertex.tangent = t;
        }
    }
}
