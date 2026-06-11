#pragma once

#include "Scene.h"
#include "..\..\Freeside.Assets\include\AssetManager.h"

#include <filesystem>


namespace Freeside::Scene
{
	class SceneSerializer
	{
	public:
		bool Save(const Scene& scene, const std::filesystem::path& path);
		bool Load(Scene& scene, const std::filesystem::path& path, Assets::AssetManager& assets);
	private:
	};
}