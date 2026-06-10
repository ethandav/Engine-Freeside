#include "..\include\SceneSerializer.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

namespace Freeside::Scene
{
    bool SceneSerializer::Load(Scene& scene, const std::filesystem::path& path)
    {
        using json = nlohmann::json;

        std::ifstream file(path);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open scene file: " + path.string());
        }

        nlohmann::json sceneJson;
        file >> sceneJson;

		return true;
	}
}