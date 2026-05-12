#pragma once

#include "MeshData.h"
#include "Materials.h"
#include "Camera.h"
#include "Lights.h"
#include "..\..\..\efg.Core\include\math\Mat4.h"

#include <stdint.h>
#include <string>

namespace efg
{
	struct RendererDesc
	{
		void* nativeWindowHandle = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct RenderObject
	{
		MeshHandle mesh;
		MaterialHandle material;
		Math::Mat4 world = efg::Math::Mat4::Identity();
		Math::Mat4 initialTransform = efg::Math::Mat4::Identity();
		std::wstring name = L"Render Object";
	};

	struct FramePacket
	{
		uint64_t frameId = 0;
		Camera camera = {};
		Lights::Directional directionalLight = {};
		std::vector<RenderObject> renderObjects = {};
		std::vector<efg::Lights::Point> pointLights = {};
	};

	struct ObjectConstants
	{
		Math::Mat4 world;
	};

	struct MaterialConstants
	{
		Math::Vec4 baseColor;
		Math::Vec4 specular;
	};
}