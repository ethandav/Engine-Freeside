#include "..\..\..\include\d3d12\libraries\D3D12MaterialLibrary.h"
#include "..\..\..\..\efg.Core\include\math\Mat4.h"

#include <stdexcept>

namespace efg::d3d12
{
	Freeside::MaterialHandle D3D12MaterialLibrary::RegisterMaterial(const Material& material)
	{
		m_materials.push_back(material);

		return Freeside::MaterialHandle
		{
			static_cast<uint32_t>(m_materials.size() - 1)
		};
	}

	const Material& D3D12MaterialLibrary::GetMaterialByHandle(Freeside::MaterialHandle handle) const
	{
		if (!handle.IsValid() || handle.index >= m_materials.size())
		{
			throw std::runtime_error("Invalid material handle.");
		}

		return m_materials[handle.index];
	}

	Material D3D12MaterialLibrary::GetDefaultMaterial()
	{
		Material material = {};
		MaterialConstants constants{
			Freeside::Math::Vec4(0.8f, 0.8f, 0.8f, 1.0f),
			Freeside::Math::Vec4(1.0f, 64.0f, 0.0f, 0.0f),
		};
		material.constants = constants;
		return material;
	}
}