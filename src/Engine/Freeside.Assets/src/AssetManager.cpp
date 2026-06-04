#include "..\include\AssetManager.h"

namespace Freeside::Assets
{
	void AssetManager::Initialize(Renderer* renderer)
	{
		m_renderer = renderer;
	}

	MeshHandle AssetManager::CreateMesh(MeshDesc desc)
	{
		return m_renderer->CreateMesh(desc);
	}

	MaterialHandle AssetManager::CreateMaterial(MaterialDesc desc)
	{
		return m_renderer->RegisterMaterial(desc);
	}

	ImportedModel AssetManager::ImportModel(const std::filesystem::path& path)
	{
		return m_modelImporter.ImportModel(path);
	}

	TextureHandle AssetManager::CreateTextureFromImagePath(const std::filesystem::path& path)
	{
		TextureDesc image = m_imageLoader.LoadImageWithWIC(path.c_str());
		return CreateTexture(image);
	}

	TextureHandle AssetManager::CreateTexture(const TextureDesc& desc)
	{
		return m_renderer->CreateMaterialTexture2d(desc);
	}

    MaterialDesc AssetManager::ConvertGLTFMaterial(const ImportedMaterial& src, const std::vector<TextureDesc>& textures)
    {
        MaterialDesc dst = {};

        dst.baseColorFactor = src.baseColorFactor;
        dst.metallicFactor = src.metallicFactor;
        dst.roughnessFactor = src.roughnessFactor;
        dst.alphaCutoff = src.alphaCutoff;

        if (src.baseColorTexture >= 0)
            dst.baseColorTexture = CreateTexture(textures[src.baseColorTexture]);
        if (src.normalTexture >= 0)
            dst.normalTexture = CreateTexture(textures[src.normalTexture]);
        if (src.metallicRoughnessTexture >= 0)
            dst.metallicRoughnessTexture = CreateTexture(textures[src.metallicRoughnessTexture]);
        if (src.emissiveTexture >= 0)
            dst.emissiveTexture = CreateTexture(textures[src.emissiveTexture]);
		if (src.occlusionTexture >= 0)
			dst.occlusionTexture = CreateTexture(textures[src.occlusionTexture]);

        return dst;
    }
}