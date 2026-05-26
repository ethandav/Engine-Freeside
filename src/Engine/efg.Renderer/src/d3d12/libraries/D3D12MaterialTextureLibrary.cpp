#include "..\..\..\include\d3d12\libraries\D3D12MaterialTextureLibrary.h"

#include <stdexcept>
#include <cstdint>

namespace efg::d3d12
{
	Freeside::TextureHandle D3D12MaterialTextureLibrary::RegisterMaterialTexture2D(const GpuTexture2D& texture)
	{
		m_textures2D.push_back(texture);

		return Freeside::TextureHandle
		{
			static_cast<uint32_t>(m_textures2D.size() - 1)
		};
	}

	Freeside::TextureHandle D3D12MaterialTextureLibrary::RegisterTextureCube(const GpuTextureCube& texture)
	{
		m_textureCubes.push_back(texture);

		return Freeside::TextureHandle
		{
			static_cast<uint32_t>(m_textureCubes.size() - 1)
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

	void D3D12MaterialTextureLibrary::RegisterDefaultHeightTexture2D(const GpuTexture2D& texture)
	{
		m_defaultHeightTexture = texture;
	}

	void D3D12MaterialTextureLibrary::RegisterDefaultSkyboxTexture(const GpuTextureCube& texture)
	{
		m_defaultSkybox = texture;
	}

	const GpuTexture2D& D3D12MaterialTextureLibrary::GetTexture2DByHandle(Freeside::TextureHandle handle) const
	{
		if (!handle.IsValid() || handle.index >= m_textures2D.size())
		{
			throw std::runtime_error("Invalid material handle.");
		}

		return m_textures2D[handle.index];
	}

	const GpuTextureCube& D3D12MaterialTextureLibrary::GetTextureCubeByHandle(Freeside::TextureHandle handle) const
	{
		if (!handle.IsValid() || handle.index >= m_textureCubes.size())
		{
			throw std::runtime_error("Invalid material handle.");
		}

		return m_textureCubes[handle.index];
	}

	GpuTexture2D D3D12MaterialTextureLibrary::GetDefaultMaterialTexture()
	{
		return m_defaultTexture;
	}

	GpuTexture2D D3D12MaterialTextureLibrary::GetDefaultNormalTexture()
	{
		return m_defaultNormalTexture;
	}

	GpuTexture2D D3D12MaterialTextureLibrary::GetDefaultHeightTexture()
	{
		return m_defaultHeightTexture;
	}

	GpuTextureCube D3D12MaterialTextureLibrary::GetDefaulSkyboxTexture()
	{
		return m_defaultSkybox;
	}
}