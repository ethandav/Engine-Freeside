#pragma once

#include "..\libraries\D3D12MeshLibrary.h"
#include "..\libraries\D3D12MaterialLibrary.h"
#include "..\libraries\D3D12MaterialTextureLibrary.h"

namespace efg::d3d12
{
    struct D3D12RenderResourceLibraries
    {
        D3D12MeshLibrary* meshes = nullptr;
        D3D12MaterialLibrary* materials = nullptr;
        D3D12MaterialTextureLibrary* textures = nullptr;
    };
}