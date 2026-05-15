#pragma once

#include <cstdint>

namespace Freeside
{
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

	struct MeshHandle
	{
		uint32_t index = UINT32_MAX;

		bool IsValid() const
		{
			return index != UINT32_MAX;
		}

		bool operator==(const MeshHandle& other) const
		{
			return index == other.index;
		}

		bool operator!=(const MeshHandle& other) const
		{
			return !(*this == other);
		}
	};
}