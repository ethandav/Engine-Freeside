#pragma once

#include "..\..\..\..\..\efg.Core\include\math\Vec2.h"
#include "..\..\..\..\..\efg.Core\include\math\Vec4.h"
#include "..\..\..\..\..\efg.Core\include\math\Mat4.h"
#include "..\..\..\render\types\Handles.h"
#include "..\..\..\render\RenderQueue.h"
#include "..\ShadowMap\D3D12ShadowMapRenderPassTypes.h"

namespace efg::d3d12
{
	struct ForwardLitGeometryRenderPassInputs
	{
		const RenderQueue* renderQueue = nullptr;
		const ShadowMapFrameData* shadowMapData = nullptr;
	};

	struct ForwardLitGeometryRenderPassOutputs
	{

	};

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

	struct GpuPointLight
	{
		Freeside::Math::Vec4 positionAndRadius;
		Freeside::Math::Vec4 colorAndIntensity;
	};

	struct GpuDirectionalLight
	{
		Freeside::Math::Vec4 directionAndIntensity;
		Freeside::Math::Vec4 colorAndPadding;
	};

	struct ShadowConstants
	{
		Freeside::Math::Mat4 LightViewProjection;
		Freeside::Math::Vec4 ShadowParams; // x = bias, y = strength, etc.
	};
}