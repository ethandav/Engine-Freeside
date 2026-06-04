#pragma once

#include "..\..\efg\include\render\types\Handles.h"
#include "..\..\efg\include\render\types\MeshTypes.h"
#include "..\..\efg\include\render\Renderer.h"
#include "..\..\Freeside.Assets\include\ModelImporter.h"
#include "ImageLoader.h"
#include "MaterialTypes.h"

namespace Freeside::Assets
{
	class AssetManager
	{
	public:
		void Initialize(Renderer* renderer);
		MeshHandle CreateMesh(MeshDesc desc);
		MaterialHandle CreateMaterial(MaterialDesc desc);
		ImportedModel ImportModel(const std::filesystem::path& path);
		TextureHandle CreateTextureFromImagePath(const std::filesystem::path& path);
		TextureHandle CreateTexture(const TextureDesc& desc);
	private:
		Renderer* m_renderer = nullptr;
		ModelImporter m_modelImporter;
		ImageLoader m_imageLoader;
	};
}