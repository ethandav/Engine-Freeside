#pragma once

#include "Handles.h"
#include "..\..\..\..\efg.Core\include\math\Mat4.h"
#include "MeshTypes.h"

#include <string>

namespace Freeside
{
	struct RenderObject
	{
		MeshHandle mesh;
		MaterialHandle material;
		Freeside::Math::Mat4 world = Freeside::Math::Mat4::Identity();
		Freeside::Math::Mat4 initialTransform = Freeside::Math::Mat4::Identity();
		Transform transform;
		std::wstring name = L"Render Object";
	};
}