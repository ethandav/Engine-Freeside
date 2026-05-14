#include "..\..\..\include\d3d12\libraries\D3D12TextureLibrary.h"

#include <stdexcept>
#include <cstdint>

namespace efg::d3d12
{
	TextureHandle D3D12TextureLibrary::RegisterTexture2D(const GpuTexture2D& texture)
	{
		m_textures.push_back(texture);

		return efg::TextureHandle
		{
			static_cast<uint32_t>(m_textures.size() - 1)
		};
	}

	const GpuTexture2D& D3D12TextureLibrary::GetTextureByHandle(TextureHandle handle) const
	{
		if (!handle.IsValid() || handle.index >= m_textures.size())
		{
			throw std::runtime_error("Invalid material handle.");
		}

		return m_textures[handle.index];
	}

	GpuTexture2D D3D12TextureLibrary::GetDefaultWhiteTexture()
	{
		return GpuTexture2D();
	}
}