#pragma once
#include "..\..\render\types\Handles.h"
#include "..\resources\D3D12GpuTexture.h"

#include <vector>

namespace efg::d3d12
{
    class D3D12MaterialTextureLibrary
    {
    public:
        Freeside::TextureHandle RegisterMaterialTexture2D(const GpuTexture2D& desc);
        void RegisterDefaultMaterialTexture2D(const GpuTexture2D& texture);
        void RegisterDefaultNormalTexture2D(const GpuTexture2D& texture);
        const GpuTexture2D& GetTextureByHandle(Freeside::TextureHandle handle) const;
        GpuTexture2D GetDefaultMaterialTexture();
        GpuTexture2D GetDefaultNormalTexture();
    private:
        std::vector<GpuTexture2D> m_textures;
        GpuTexture2D m_defaultTexture;
        GpuTexture2D m_defaultNormalTexture;
    };
}