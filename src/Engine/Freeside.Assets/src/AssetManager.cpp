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
}