#pragma once

#include "Scene.h"

#include <filesystem>

namespace Freeside::Scene
{
	class SceneSerializer
	{
	public:
		static bool Save(const Scene& scene, const std::filesystem::path& path);
		static bool Load(Scene& scene, const std::filesystem::path& path);
	private:
	};
}