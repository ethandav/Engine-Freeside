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
		MaterialHandle handle = m_renderer->RegisterMaterial(desc);
		m_registeredMaterials[handle.index] = desc;

		return handle;
	}

	MaterialHandle AssetManager::CreateDefaultMaterial()
	{
		MaterialDesc desc = MaterialDesc();
		MaterialHandle handle = m_renderer->RegisterMaterial(desc);
		m_registeredMaterials[handle.index] = desc;

		return handle;
	}

	MaterialDesc* AssetManager::GetRegisteredMaterial(MaterialHandle handle)
	{
		return &m_registeredMaterials[handle.index];
	}

	void AssetManager::UpdateMaterial(MaterialHandle handle)
	{
		MaterialDesc* desc = GetRegisteredMaterial(handle);
		m_renderer->UpdateMaterial(handle, *desc);
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

	TextureHandle AssetManager::CreateSkyboxFromImagePaths(const std::array<std::wstring, 6>& paths)
	{
		std::array<TextureDesc, 6> faces =
		{
			m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/right.jpg"),
			m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/left.jpg"),
			m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/top.jpg"),
			m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/bottom.jpg"),
			m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/front.jpg"),
			m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/back.jpg"),
		};

		return CreateTextureCube(faces);
	}

	TextureHandle AssetManager::CreateTexture(const TextureDesc& desc)
	{
		return m_renderer->CreateMaterialTexture2d(desc);
	}

	TextureHandle AssetManager::CreateTextureCube(const std::array<TextureDesc, 6> faces)
	{
		return m_renderer->CreateTextureCube(faces);
	}

    MaterialDesc AssetManager::ConvertGLTFMaterial(const ImportedMaterial& src, const std::vector<TextureDesc>& textures)
    {
        MaterialDesc dst = {};

        dst.baseColorFactor = src.baseColorFactor;
        dst.metallicFactor = src.metallicFactor;
        dst.roughnessFactor = src.roughnessFactor;
        dst.alphaCutoff = src.alphaCutoff;

		if (src.baseColorTexture >= 0)
		{
			TextureDesc desc = textures[src.baseColorTexture];
			desc.format = Format::R8G8B8A8_UNorm_SRGB;
            dst.baseColorTexture = CreateTexture(desc);
		}
		if (src.normalTexture >= 0)
		{
			TextureDesc desc = textures[src.normalTexture];
            dst.normalTexture = CreateTexture(textures[src.normalTexture]);
		}
		if (src.metallicRoughnessTexture >= 0)
		{
            dst.metallicRoughnessTexture = CreateTexture(textures[src.metallicRoughnessTexture]);
		}
		if (src.emissiveTexture >= 0)
		{
			TextureDesc desc = textures[src.emissiveTexture];
			desc.format = Format::R8G8B8A8_UNorm_SRGB;
            dst.emissiveTexture = CreateTexture(desc);
		}
		if (src.occlusionTexture >= 0)
		{
			dst.occlusionTexture = CreateTexture(textures[src.occlusionTexture]);
		}

		if (src.alphaMode == "MASK")
			dst.alphaMode = Freeside::AlphaMode::Mask;
		else if (src.alphaMode == "BLEND")
			dst.alphaMode = Freeside::AlphaMode::Blend;
		else
			dst.alphaMode = Freeside::AlphaMode::Opaque;

        return dst;
    }
}