#pragma once

#include "..\..\render\RenderTypes.h"

#include <vector>

namespace efg::d3d12
{
    class D3D12MaterialLibrary
    {
    public:
        MaterialHandle RegisterMaterial(const MaterialDesc& desc);
        const Material& GetMaterialByHandle(MaterialHandle handle) const;
        Material GetDefaultMaterial();
    private:
        std::vector<Material> m_materials = {};
    };
}