#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "InputState.h"

#include <Windows.h>
#include <string>

namespace Freeside
{
	class Window
	{
	public:
		void Create(HINSTANCE hInstance, UINT width = 1920, UINT height = 1080, std::wstring = L"Test App");
		void Show(int nCmdShow);
		void PollEvents();
		bool IsOpen();
		HWND GetHwnd();
		InputState PollInput();
		bool IsCursorLocked() const { return m_cursorLocked; };
		void SetCursorLocked(bool locked);

	private:
		bool m_isOpen = false;
		HWND m_hwnd = nullptr;
		InputState m_input;
		bool m_cursorLocked = false;

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
		void UpdateCursorClip();
		void RegisterRawMouseInput();
	};

}