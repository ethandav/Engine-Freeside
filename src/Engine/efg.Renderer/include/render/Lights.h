#pragma once
#include "..\..\..\efg.Core\include\math\Vec3.h"
#include "..\..\..\efg.Core\include\math\Vec4.h"

#include <cstdint>

namespace Freeside
{
	namespace Lights
	{
		static constexpr uint32_t MaxPointLights = 1024;

		struct DirectionalLightConstants
		{
			Freeside::Math::Vec4 directionAndIntensity; // xyz = direction, w = intensity
			Freeside::Math::Vec4 colorAndPadding;       // rgb = color, w unused
		};

		struct GpuPointLight
		{
			Freeside::Math::Vec4 positionAndRadius;
			Freeside::Math::Vec4 colorAndIntensity;
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

			Freeside::Math::Vec3 direction = Freeside::Math::Vec3(0.0f, 01.0f, 0.0f);
			Freeside::Math::Vec3 color = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
		};

		class Point
		{
		public:
			Point() = default;
			void BuildGpuPointLights();

			Freeside::Math::Vec3 position = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
			float radius = 0.0f;
			Freeside::Math::Vec3 color = Freeside::Math::Vec3(1.0f, 1.0f, 1.0f);
			float intensity = 1.0f;
		};
	}
}