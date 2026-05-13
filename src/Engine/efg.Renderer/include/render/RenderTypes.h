#pragma once

#include "MeshData.h"
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

	struct MaterialHandle
	{
		uint32_t index = UINT32_MAX;

		bool IsValid() const
		{
			return index != UINT32_MAX;
		}

		bool operator==(const MaterialHandle& other) const
		{
			return index == other.index;
		}

		bool operator!=(const MaterialHandle& other) const
		{
			return !(*this == other);
		}
	};

	struct TextureHandle
	{
		uint32_t index = UINT32_MAX;

		bool IsValid() const
		{
			return index != UINT32_MAX;
		}
	};

	struct MaterialDesc
	{
		Math::Vec3 baseColor = Math::Vec3(0.8f, 0.8f, 0.8f);
		Math::Vec2 specular = Math::Vec2(1.0f, 64.0f);
		TextureHandle baseColorTexture2D = {};
	};

	struct MaterialConstants
	{
		Math::Vec4 baseColor;
		Math::Vec4 specular;
	};

	struct Material
	{
		MaterialConstants constants = {};
		TextureHandle baseColorTexture = {};
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

	struct InstanceData
	{
		efg::Math::Mat4 world;
	};
}