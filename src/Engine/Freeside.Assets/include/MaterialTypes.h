#pragma once

#include "Format.h"
#include "..\..\efg\include\render\types\Handles.h"
#include "..\..\Freeside.Core\include\math\Vec4.h"
#include "..\..\Freeside.Core\include\math\Vec3.h"
#include "..\..\Freeside.Core\include\math\Vec2.h"

#include <string>
#include <vector>

namespace Freeside
{
    enum class AlphaMode : uint8_t
    {
        Opaque,
        Mask,
        Blend
    };

    enum MaterialFlags : uint32_t
    {
        MaterialFlag_HasBaseColorTexture = 1 << 0,
        MaterialFlag_HasNormalTexture = 1 << 1,
        MaterialFlag_HasMetallicRoughnessTexture = 1 << 2,
        MaterialFlag_HasOcclusionTexture = 1 << 3,
        MaterialFlag_HasEmissiveTexture = 1 << 4,
        MaterialFlag_HasHeightTexture = 1 << 5,
        MaterialFlag_AlphaMask = 1 << 6,
        MaterialFlag_DoubleSided = 1 << 7
    };

    struct MaterialDesc
    {
        // Base color / albedo.
        Math::Vec4 baseColorFactor = Math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        TextureHandle baseColorTexture = {};

        // glTF metallic-roughness model.
        float metallicFactor = 1.0f;
        float roughnessFactor = 1.0f;
        TextureHandle metallicRoughnessTexture = {};

        // Normal mapping.
        TextureHandle normalTexture = {};
        float normalScale = 1.0f;

        // Ambient occlusion.
        TextureHandle occlusionTexture = {};
        float occlusionStrength = 1.0f;

        // Emissive.
        Math::Vec3 emissiveFactor = Math::Vec3(0.0f, 0.0f, 0.0f);
        TextureHandle emissiveTexture = {};

        // Your custom/non-glTF extension slot.
        TextureHandle heightTexture = {};
        float heightScale = 0.0f;

        // Tiling. glTF default is 1,1. Later this should come from KHR_texture_transform.
        Math::Vec2 uvScale = Math::Vec2(1.0f, 1.0f);
        Math::Vec2 uvOffset = Math::Vec2(0.0f, 0.0f);

        // Alpha/render-state-ish material data.
        AlphaMode alphaMode = AlphaMode::Opaque;
        float alphaCutoff = 0.5f;
        bool doubleSided = false;

        // Temporary compatibility with your current Phong shader.
        // Eventually remove this once you move fully to PBR.
        Math::Vec2 specular = Math::Vec2(1.0f, 64.0f);
    };

	struct TextureDesc
	{
		std::vector<uint8_t> pixels;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t rowPitch = 0;
		Format format = Format::Unknown;
		std::string name;
	};
}