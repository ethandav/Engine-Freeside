#pragma once

#include "ImportedAssetTypes.h"
#include <filesystem>

namespace Freeside::Assets
{
	class ModelImporter
	{
	public:
        ImportedModel ImportModel(const std::filesystem::path& path);
	private:
	};
}