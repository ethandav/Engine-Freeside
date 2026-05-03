#include <iostream>
#include <Windows.h>
#include <pix3.h>

#include "..\include\Win32Application.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

    HMODULE module = LoadLibraryW(L"WinPixEventRuntime.dll");

    if (!module)
    {
        DWORD error = GetLastError();
        throw std::runtime_error("Failed to load WinPixEventRuntime.dll");
    }
    PIXLoadLatestWinPixGpuCapturerLibrary();
	Application app;
	app.Run(hInstance, nCmdShow);
}