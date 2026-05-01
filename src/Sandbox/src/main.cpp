#include <iostream>
#include <Windows.h>

#include "..\include\Win32Application.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	Application app;
	app.Run(hInstance, nCmdShow);
}