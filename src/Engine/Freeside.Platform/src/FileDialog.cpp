#include "..\include\FileDialog.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <commdlg.h>

namespace Freeside
{
    std::optional<std::filesystem::path> OpenFileDialog(void* ownerWindow, const wchar_t* filter)
    {
        wchar_t fileName[MAX_PATH] = {};

        OPENFILENAMEW ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = static_cast<HWND>(ownerWindow);
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags =
            OFN_PATHMUSTEXIST |
            OFN_FILEMUSTEXIST |
            OFN_NOCHANGEDIR;

        if (GetOpenFileNameW(&ofn))
        {
            return std::filesystem::path(fileName);
        }

        return std::nullopt;
    }
}