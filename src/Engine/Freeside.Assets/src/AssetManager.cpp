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
		return m_renderer->CreateMaterialTexture2d(image);
	}
}