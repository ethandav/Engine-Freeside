#pragma once

#include "..\render\RenderTypes.h"
#include "D3D12BufferData.h"

namespace efg::d3d12
{
    class D3D12TextureLibrary
    {
    public:
        TextureHandle RegisterTexture2D(const GpuTexture2D& desc);
        const GpuTexture2D& GetTextureByHandle(TextureHandle handle) const;
        GpuTexture2D GetDefaultWhiteTexture();
    private:
        std::vector<GpuTexture2D> m_textures;
    };
}