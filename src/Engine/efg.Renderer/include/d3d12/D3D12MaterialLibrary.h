#pragma once
#include "..\render\Materials.h"
#include "..\..\..\efg.Core\include\math\Vec4.h"

#include <vector>

namespace efg::d3d12
{
    struct GpuMaterial
    {
        efg::Math::Vec4 baseColor = efg::Math::Vec4(0.8f, 0.8f, 0.8f, 0.0f);
        efg::Math::Vec4 specular = efg::Math::Vec4(1.0f, 64.0f, 0.0f, 0.0f);
    };

    class D3D12MaterialLibrary
    {
    public:
        efg::MaterialHandle RegisterMaterial(const efg::MaterialDesc& desc);
        const GpuMaterial& GetMaterialByHandle(efg::MaterialHandle handle) const;
        GpuMaterial GetDefaultMaterial();
    private:
        std::vector<GpuMaterial> m_materials = {};
    };
}