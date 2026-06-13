#pragma once

#include "..\..\efg\include\render\types\Handles.h"
#include "..\..\efg\include\render\types\MeshTypes.h"
#include "..\..\efg\include\render\Renderer.h"
#include "..\..\Freeside.Assets\include\ModelImporter.h"
#include "ImageLoader.h"
#include "MaterialTypes.h"

#include <array>
#include <unordered_map>

namespace Freeside::Assets
{
	class AssetManager
	{
	public:
		void Initialize(Renderer* renderer);
		MeshHandle CreateMesh(MeshDesc desc);
		MaterialHandle CreateMaterial(MaterialDesc desc);
		MaterialHandle CreateDefaultMaterial();
		MaterialDesc* GetRegisteredMaterial(MaterialHandle handle);
		void UpdateMaterial(MaterialHandle handle);
		ImportedModel ImportModel(const std::filesystem::path& path);
		TextureHandle CreateTextureFromImagePath(const std::filesystem::path& path);
		TextureHandle CreateSkyboxFromImagePaths(const std::array<std::wstring, 6>& paths);
		TextureHandle CreateTexture(const TextureDesc& desc);
		TextureHandle CreateTextureCube(const std::array<TextureDesc, 6> faces);
		MaterialDesc ConvertGLTFMaterial(const ImportedMaterial& src, const std::vector<TextureDesc>& textures);
	private:

		Renderer* m_renderer = nullptr;
		ModelImporter m_modelImporter;
		ImageLoader m_imageLoader;

		std::unordered_map<uint32_t, MaterialDesc> m_registeredMaterials = {};
	};
}