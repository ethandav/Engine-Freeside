#pragma once

#include "..\..\..\..\efg.Core\include\math\Vec4.h"
#include "..\..\..\..\efg.Core\include\math\Mat4.h"
#include "..\..\render\types\Handles.h"

namespace efg
{
	struct InstanceData
	{
		Freeside::Math::Mat4 world;
	};

	struct MaterialConstants
	{
		Freeside::Math::Vec4 baseColor;
		Freeside::Math::Vec4 specular;
	};

	struct Material
	{
		MaterialConstants constants = {};
		Freeside::TextureHandle baseColorTexture = {};
	};
}