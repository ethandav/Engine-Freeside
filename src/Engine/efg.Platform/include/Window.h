#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <string>

struct WindowProperties
{
	UINT width = 0;
	UINT height = 0;
	std::wstring name = L"";
};

class Window
{
public:
	void Create(HINSTANCE hInstance, UINT width = 1920, UINT height = 1080, std::wstring = L"Test App");
	void Show(int nCmdShow);
	void PollEvents();
	bool IsOpen();

private:
	bool m_isOpen = false;
	HWND m_hwnd = nullptr;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
};
