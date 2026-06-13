#pragma once

#include "..\..\..\..\Freeside.Assets\include\MaterialTypes.h"
#include "..\..\render\types\Handles.h"
#include "..\passes\ForwardLitGeometry\D3D12ForwardLitGeometryTypes.h"

#include <vector>

namespace efg::d3d12
{
    class D3D12MaterialLibrary
    {
    public:
        Freeside::MaterialHandle RegisterMaterial(const Material& material);
        const Material& GetMaterialByHandle(Freeside::MaterialHandle handle) const;
        void UpdateMaterial(Freeside::MaterialHandle handle, Material material);
        Material GetDefaultMaterial();
    private:
        std::vector<Material> m_materials = {};
        Material m_defaultMaterial = {};
    };
}