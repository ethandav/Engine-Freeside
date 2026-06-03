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
        Freeside::TextureHandle RegisterTextureCube(const GpuTextureCube& desc);
        void RegisterDefaultMaterialTexture2D(const GpuTexture2D& texture);
        void RegisterDefaultNormalTexture2D(const GpuTexture2D& texture);
        void RegisterDefaultHeightTexture2D(const GpuTexture2D& texture);
        void RegisterDefaultSkyboxTexture(const GpuTextureCube& texture);
        const GpuTexture2D& GetTexture2DByHandle(Freeside::TextureHandle handle) const;
        const GpuTextureCube& GetTextureCubeByHandle(Freeside::TextureHandle handle) const;
        GpuTexture2D GetDefaultMaterialTexture();
        GpuTexture2D GetDefaultNormalTexture();
        GpuTexture2D GetDefaultHeightTexture();
        GpuTextureCube GetDefaulSkyboxTexture();
    private:
        std::vector<GpuTexture2D> m_textures2D;
        std::vector<GpuTextureCube> m_textureCubes;
        GpuTexture2D m_defaultTexture;
        GpuTexture2D m_defaultNormalTexture;
        GpuTexture2D m_defaultHeightTexture;
        GpuTextureCube m_defaultSkybox;
    };
}