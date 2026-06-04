#pragma once

#include "Format.h"
#include "..\..\efg\include\render\types\Handles.h"
#include "..\..\Freeside.Core\include\math\Vec3.h"
#include "..\..\Freeside.Core\include\math\Vec2.h"

#include <string>
#include <vector>

namespace Freeside
{
	struct MaterialDesc
	{
		Freeside::Math::Vec3 baseColor = Freeside::Math::Vec3(0.8f, 0.8f, 0.8f);
		Freeside::Math::Vec2 specular = Freeside::Math::Vec2(1.0f, 64.0f);
		Freeside::Math::Vec2 uvScale = Freeside::Math::Vec2(1.0f, 1.0f);
		TextureHandle baseColorTexture = {};
		TextureHandle normalTexture = {};
		TextureHandle heightTexture = {};
	};

	struct TextureDesc
	{
		std::vector<uint8_t> pixels;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t rowPitch = 0;
		Format format = Format::Unknown;
	};
}