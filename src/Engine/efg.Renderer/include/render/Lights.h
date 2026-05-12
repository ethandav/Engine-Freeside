#pragma once
#include "..\..\..\efg.Core\include\math\Vec3.h"
#include "..\..\..\efg.Core\include\math\Vec4.h"

#include <cstdint>

namespace efg
{
	namespace Lights
	{
		static constexpr uint32_t MaxPointLights = 1024;

		struct DirectionalLightConstants
		{
			Vec4 directionAndIntensity; // xyz = direction, w = intensity
			Vec4 colorAndPadding;       // rgb = color, w unused
		};

		struct GpuPointLight
		{
			Vec4 positionAndRadius;
			Vec4 colorAndIntensity;
		};

		struct PointLightConstants
		{
			uint32_t pointLightCount = 0;
			float padding[3] = {};
		};

		class Directional
		{
		public:
			Directional() = default;
			DirectionalLightConstants BuildDirectionalLightConstants() const;

			Vec3 direction = Vec3(0.0f, 01.0f, 0.0f);
			Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
		};

		class Point
		{
		public:
			Point() = default;
			void BuildGpuPointLights();

			Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
			float radius = 0.0f;
			Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
			float intensity = 1.0f;
		};
	}
}