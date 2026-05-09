#include "..\..\include\d3d12\D3D12MaterialLibrary.h"

#include <stdexcept>

efg::MaterialHandle D3D12MaterialLibrary::RegisterMaterial(const efg::MaterialDesc& mat)
{
	GpuMaterial material{
		efg::Vec4(mat.baseColor.x, mat.baseColor.y, mat.baseColor.z, 1.0f),
		efg::Vec4(mat.specular.x, mat.specular.y, 0.0f, 0.0f),
	};
	m_materials.push_back(material);

	return efg::MaterialHandle
	{
		static_cast<uint32_t>(m_materials.size() - 1)
	};
}

const GpuMaterial& D3D12MaterialLibrary::GetMaterialByHandle(efg::MaterialHandle handle) const
{
	if (!handle.IsValid() || handle.index >= m_materials.size())
	{
		throw std::runtime_error("Invalid material handle.");
	}

	return m_materials[handle.index];
}

const GpuMaterial& D3D12MaterialLibrary::GetDefaultMaterial() const
{
	return GpuMaterial {
		efg::Vec4(0.8f, 0.8f, 0.8f, 1.0f),
		efg::Vec4(1.0f, 64.0f, 0.0f, 0.0f),
	};
}
