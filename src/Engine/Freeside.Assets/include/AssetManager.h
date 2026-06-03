#pragma once

#include "..\..\efg\include\render\types\Handles.h"
#include "..\..\efg\include\render\types\MeshTypes.h"
#include "..\..\efg\include\render\types\MaterialTypes.h"
#include "..\..\efg\include\render\Renderer.h"
#include "..\..\Freeside.Assets\include\ModelImporter.h"

namespace Freeside::Assets
{
	class AssetManager
	{
	public:
		void Initialize(Renderer* renderer);
		MeshHandle CreateMesh(MeshDesc desc);
		MaterialHandle CreateMaterial(MaterialDesc desc);
		ImportedModel ImportModel(const std::filesystem::path& path);
	private:
		Renderer* m_renderer = nullptr;
		ModelImporter m_modelImporter;
	};
}