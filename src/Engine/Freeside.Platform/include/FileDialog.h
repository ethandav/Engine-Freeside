#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace Freeside
{
    std::optional<std::filesystem::path> OpenFileDialog(void* ownerWindow, const wchar_t* filter);
}