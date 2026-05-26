#pragma once

#include "..\..\..\..\..\efg.Core\include\math\Vec2.h"
#include "..\..\..\..\..\efg.Core\include\math\Vec4.h"
#include "..\..\..\..\..\efg.Core\include\math\Mat4.h"
#include "..\..\..\render\types\Handles.h"
#include "..\..\..\render\RenderQueue.h"
#include "..\ShadowMap\D3D12ShadowMapRenderPassTypes.h"

namespace efg::d3d12
{
	struct MaterialConstants
	{
		Freeside::Math::Vec4 baseColor;
		Freeside::Math::Vec4 specular;
		Freeside::Math::Vec4 uvScale;
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
		uint32_t directionalLightCount = 0;
		float padding[3] = {};
	};

	struct PointLightConstants
	{
		uint32_t pointLightCount = 0;
		float padding[3] = {};
	};

	struct ShadowConstants
	{
		Freeside::Math::Mat4 LightViewProjection;
		Freeside::Math::Vec4 ShadowParams; // x = bias, y = strength, etc.
	};

	struct GpuDirectionalLight
	{
		Freeside::Math::Vec4 directionAndIntensity;
		Freeside::Math::Vec4 colorAndPadding;
		int32_t shadowDataIndex = -1;
		int32_t shadowTextureDescriptorIndex = -1;
		float padding[3] = {};
	};

	struct GpuPointLight
	{
		Freeside::Math::Vec4 positionAndRadius;
		Freeside::Math::Vec4 colorAndIntensity;
		int32_t shadowDataIndex = -1;
		int32_t shadowTextureDescriptorIndex = -1;
		float padding[3] = {};
	};

	struct GpuDirectionalShadowData
	{
		Freeside::Math::Mat4 lightViewProjection;
	};

	struct GpuPointShadowData
	{
		Freeside::Math::Mat4 faceViewProjection[6];
		float farPlane = 0.0f;
		float padding[3] = {};
	};

	struct ShadowMetadataConstants
	{
		uint32_t directionalShadowCount = 0;
		uint32_t pointShadowCount = 0;
		float shadowBias = 0.001f;
		float shadowStrength = 1.0f;
	};
}