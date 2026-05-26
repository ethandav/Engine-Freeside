#include "..\..\..\include\d3d12\libraries\D3D12MaterialTextureLibrary.h"

#include <stdexcept>
#include <cstdint>

namespace efg::d3d12
{
	Freeside::TextureHandle D3D12MaterialTextureLibrary::RegisterMaterialTexture2D(const GpuTexture2D& texture)
	{
		m_textures.push_back(texture);

		return Freeside::TextureHandle
		{
			static_cast<uint32_t>(m_textures.size() - 1)
		};
	}

	void D3D12MaterialTextureLibrary::RegisterDefaultMaterialTexture2D(const GpuTexture2D& texture)
	{
		m_defaultTexture = texture;
	}

	void D3D12MaterialTextureLibrary::RegisterDefaultNormalTexture2D(const GpuTexture2D& texture)
	{
		m_defaultNormalTexture = texture;
	}

	const GpuTexture2D& D3D12MaterialTextureLibrary::GetTextureByHandle(Freeside::TextureHandle handle) const
	{
		if (!handle.IsValid() || handle.index >= m_textures.size())
		{
			throw std::runtime_error("Invalid material handle.");
		}

		return m_textures[handle.index];
	}

	GpuTexture2D D3D12MaterialTextureLibrary::GetDefaultMaterialTexture()
	{
		return m_defaultTexture;
	}

	GpuTexture2D D3D12MaterialTextureLibrary::GetDefaultNormalTexture()
	{
		return m_defaultNormalTexture;
	}
}