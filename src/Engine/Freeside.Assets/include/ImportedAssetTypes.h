#pragma once

#include "..\..\efg\include\render\types\MeshTypes.h"
#include "..\..\Freeside.Core\include\math\Vec4.h"

#include <vector>
#include <string>

namespace Freeside::Assets
{
    struct ImportedPrimitive
    {
        MeshDesc meshData;
        int materialIndex = -1;
    };

    struct ImportedTexture
    {
        std::string name;
        int width = 0;
        int height = 0;
        int channels = 0;
        std::vector<uint8_t> pixels;
    };

    struct ImportedMaterial
    {
        std::string name;

        int baseColorTexture = -1;
        int normalTexture = -1;
        int metallicRoughnessTexture = -1;

        Freeside::Math::Vec4 baseColorFactor = { 1, 1, 1, 1 };
        float metallicFactor = 0.0f;
        float roughnessFactor = 1.0f;
    };

    struct ImportedModel
    {
        std::vector<ImportedPrimitive> primitives;
        std::vector<ImportedTexture> textures;
        std::vector<ImportedMaterial> materials;
    };
}