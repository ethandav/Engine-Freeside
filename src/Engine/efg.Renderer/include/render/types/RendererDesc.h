#pragma once

#include <cstdint>

namespace Freeside
{
	struct RendererDesc
	{
		void* nativeWindowHandle = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
	};
}