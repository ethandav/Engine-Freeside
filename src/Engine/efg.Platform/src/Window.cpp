#include "..\include\Window.h"

void Window::Create(HINSTANCE hInstance, UINT width, UINT height, std::wstring name)
{
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DXSampleClass";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        name.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        this);
}

void Window::Show(int nCmdShow)
{
    m_isOpen = !ShowWindow(m_hwnd, nCmdShow);
}

bool Window::IsOpen()
{
    return m_isOpen;
}

HWND Window::GetHwnd()
{
    return m_hwnd;
}

/*
To Implement:
WM_SIZE
WM_KEYDOWN
WM_KEYUP
WM_MOUSEMOVE
WM_LBUTTONDOWN
WM_LBUTTONUP
WM_RBUTTONDOWN
WM_RBUTTONUP
WM_MOUSEWHEEL
WM_SETFOCUS
WM_KILLFOCUS
*/
LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window* window = nullptr;

    if (message == WM_CREATE)
    {
        auto* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(createStruct->lpCreateParams);

        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

        window->m_hwnd = hWnd;
    }
    else
    {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->HandleMessage(message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Window::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        m_isOpen = false;
        DestroyWindow(m_hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(m_hwnd, message, wParam, lParam);
}

void Window::PollEvents()
{
    MSG msg = {};

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            m_isOpen = false;
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
