#pragma once

#include "..\..\efg\include\render\types\Handles.h"
#include "..\..\efg\include\render\types\MeshTypes.h"
#include "..\..\efg\include\render\Renderer.h"
#include "..\..\Freeside.Assets\include\ModelImporter.h"
#include "..\..\Freeside.Core\include\shapes\shapes.h"
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
		void CreateBuiltIns();
		MeshHandle CreateMesh(MeshDesc desc);
		MaterialHandle CreateMaterial(MaterialDesc desc);
		MaterialHandle CreateBuiltInMaterial();
		void CreateBuiltInMeshes();
		MaterialDesc* GetRegisteredMaterial(MaterialHandle handle);
		void UpdateMaterial(MaterialHandle handle);
		ImportedModel ImportModel(const std::filesystem::path& path);
		TextureHandle CreateTextureFromImagePath(const std::filesystem::path& path);
		TextureHandle CreateSkyboxFromImagePaths(const std::array<std::wstring, 6>& paths);
		TextureHandle CreateTexture(const TextureDesc& desc);
		TextureHandle CreateTextureCube(const std::array<TextureDesc, 6> faces);
		MaterialDesc ConvertGLTFMaterial(const ImportedMaterial& src, const std::vector<TextureDesc>& textures);

#if defined(FREESIDE_EDITOR)
		uint64_t GetTextureID(TextureHandle handle);
#endif
		
		MaterialHandle defaultMaterial;
		std::unordered_map<uint32_t, MeshHandle> shapes = {};
	private:

		Renderer* m_renderer = nullptr;
		ModelImporter m_modelImporter;
		ImageLoader m_imageLoader;

		std::unordered_map<uint32_t, MaterialDesc> m_registeredMaterials = {};

	};
}