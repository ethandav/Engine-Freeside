#pragma once

#include "..\Camera.h"
#include "..\Lights.h"
#include "..\types\RenderObject.h"

#include <cstdint>
#include <vector>

namespace efg
{
	struct FramePacket
	{
		uint64_t frameId = 0;
		Freeside::Camera camera = {};
		Freeside::Lights::Directional directionalLight = {};
		std::vector<Freeside::RenderObject> renderObjects = {};
		std::vector<Freeside::Lights::Point> pointLights = {};
	};
}