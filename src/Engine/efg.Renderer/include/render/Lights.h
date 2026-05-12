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
			Math::Vec4 directionAndIntensity; // xyz = direction, w = intensity
			Math::Vec4 colorAndPadding;       // rgb = color, w unused
		};

		struct GpuPointLight
		{
			Math::Vec4 positionAndRadius;
			Math::Vec4 colorAndIntensity;
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

			Math::Vec3 direction = Math::Vec3(0.0f, 01.0f, 0.0f);
			Math::Vec3 color = Math::Vec3(1.0f, 1.0f, 1.0f);
		};

		class Point
		{
		public:
			Point() = default;
			void BuildGpuPointLights();

			Math::Vec3 position = Math::Vec3(0.0f, 0.0f, 0.0f);
			float radius = 0.0f;
			Math::Vec3 color = Math::Vec3(1.0f, 1.0f, 1.0f);
			float intensity = 1.0f;
		};
	}
}