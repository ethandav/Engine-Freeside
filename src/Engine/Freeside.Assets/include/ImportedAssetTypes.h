#pragma once

#include "..\..\efg\include\render\types\MeshTypes.h"
#include "..\..\Freeside.Core\include\math\Vec4.h"
#include "..\..\Freeside.Core\include\math\Mat4.h"
#include "..\include\MaterialTypes.h"

#include <vector>
#include <string>

namespace Freeside::Assets
{
    struct ImportedPrimitive
    {
        MeshDesc meshData;
        int materialIndex = -1;
        Freeside::Math::Mat4 transform;
    };

    struct ImportedMesh
    {
        std::vector<ImportedPrimitive> primitives;
        std::string name;
    };

    struct ImportedNode
    {
        std::string name;
        int parentIndex = -1;
        int meshIndex = -1;
        Math::Vec3 position = {};
        Math::Quat rotation = Math::Quat::Identity();
        Math::Vec3 scale = { 1.0f, 1.0f, 1.0f };
        bool useMatrixOverride = false;
        Math::Mat4 matrixOverride = Math::Mat4::Identity();
        std::vector<int> children;
    };

    struct ImportedMaterial
    {
        std::string name;

        int metallicRoughnessTexture = -1;
        int baseColorTexture = -1;
        int normalTexture = -1;
        int occlusionTexture = -1;
        int emissiveTexture = -1;

        float metallicFactor = 0.0f;
        float roughnessFactor = 1.0f;
        Freeside::Math::Vec4 baseColorFactor = { 1, 1, 1, 1 };
        Math::Vec3 emissiveFactor = Math::Vec3(0.0f, 0.0f, 0.0f);
        float alphaCutoff = 0.5f;
        std::string alphaMode;
        bool doubleSided = false;
    };

    struct ImportedModel
    {
        std::vector<ImportedNode> nodes;
        std::vector<ImportedMesh> meshes;
        std::vector<ImportedMaterial> materials;
        std::vector<TextureDesc> textures;
        std::vector<uint32_t> rootNodes;
    };
}