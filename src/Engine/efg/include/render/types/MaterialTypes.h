#pragma once

#include "Handles.h"
#include "..\..\..\..\Freeside.Core\include\math\Vec2.h"
#include "..\..\..\..\Freeside.Core\include\math\Vec3.h"

#include <string>

namespace Freeside
{
	struct MaterialDesc
	{
		Freeside::Math::Vec3 baseColor = Freeside::Math::Vec3(0.8f, 0.8f, 0.8f);
		Freeside::Math::Vec2 specular = Freeside::Math::Vec2(1.0f, 64.0f);
		Freeside::Math::Vec2 uvScale = Freeside::Math::Vec2(1.0f, 1.0f);
		TextureHandle baseColorTexture2D = {};
		std::wstring baseColorTexturePath = {};
		std::wstring normalTexturePath = {};
		std::wstring heightTexturePath = {};
	};
}