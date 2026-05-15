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

	struct CameraConstants
	{
		Freeside::Math::Vec4 viewPosition;
		Freeside::Math::Mat4 viewProjection;
	};

	struct DirectionalLightConstants
	{
		Freeside::Math::Vec4 directionAndIntensity; // xyz = direction, w = intensity
		Freeside::Math::Vec4 colorAndPadding;       // rgb = color, w unused
	};

	struct PointLightConstants
	{
		uint32_t pointLightCount = 0;
		float padding[3] = {};
	};

	struct GpuPointLight
	{
		Freeside::Math::Vec4 positionAndRadius;
		Freeside::Math::Vec4 colorAndIntensity;
	};
}