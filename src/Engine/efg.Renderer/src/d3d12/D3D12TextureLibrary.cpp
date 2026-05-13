#include "..\..\include\d3d12\D3D12TextureLibrary.h"

namespace efg::d3d12
{
	TextureHandle D3D12TextureLibrary::RegisterTexture(const TextureDesc& desc)
	{
		return TextureHandle();
	}

	const GpuTexture2D& D3D12TextureLibrary::GetTextureByHandle(TextureHandle handle) const
	{
		return GpuTexture2D();
	}

	GpuTexture2D D3D12TextureLibrary::GetDefaultTexture2D()
	{
		return GpuTexture2D();
	}
}