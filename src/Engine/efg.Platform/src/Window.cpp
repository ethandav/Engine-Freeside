#include "..\include\Window.h"

#include <windowsx.h>

namespace Freeside
{
    static Freeside::Key TranslateVirtualKey(WPARAM virtualKey)
    {
        switch (virtualKey)
        {
        case 'W': return Freeside::Key::W;
        case 'A': return Freeside::Key::A;
        case 'S': return Freeside::Key::S;
        case 'D': return Freeside::Key::D;

        case 'Q': return Freeside::Key::Q;
        case 'E': return Freeside::Key::E;

        case VK_SPACE:   return Freeside::Key::Space;
        case VK_ESCAPE:  return Freeside::Key::Escape;
        case VK_SHIFT:   return Freeside::Key::LeftShift;
        case VK_CONTROL: return Freeside::Key::LeftCtrl;

        default:
            return Freeside::Key::Unknown;
        }
    }

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

    InputState Window::PollInput()
    {
        m_input.mouseDeltaX = 0.0f;
        m_input.mouseDeltaY = 0.0f;

        PollEvents();

        return m_input;
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
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {

            Freeside::Key key = TranslateVirtualKey(wParam);
            if (key != Freeside::Key::Unknown)
            {
                m_input.keysDown[static_cast<size_t>(key)] = true;
            }
            return 0;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {

            Freeside::Key key = TranslateVirtualKey(wParam);
            if (key != Freeside::Key::Unknown)
            {
                m_input.keysDown[static_cast<size_t>(key)] = false;
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            m_input.mouseButtonsDown[static_cast<size_t>(Freeside::MouseButton::Left)] = true;
            return 0;
        }

        case WM_LBUTTONUP:
        {
            m_input.mouseButtonsDown[static_cast<size_t>(Freeside::MouseButton::Left)] = false;
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            m_input.mouseButtonsDown[static_cast<size_t>(Freeside::MouseButton::Right)] = true;
            return 0;
        }

        case WM_RBUTTONUP:
        {
            m_input.mouseButtonsDown[static_cast<size_t>(Freeside::MouseButton::Right)] = false;
            return 0;
        }

        case WM_MBUTTONDOWN:
        {
            m_input.mouseButtonsDown[static_cast<size_t>(Freeside::MouseButton::Middle)] = true;
            return 0;
        }

        case WM_MBUTTONUP:
        {
            m_input.mouseButtonsDown[static_cast<size_t>(Freeside::MouseButton::Middle)] = false;
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            m_input.mouseX = x;
            m_input.mouseY = y;

            if (!m_hasLastMousePosition)
            {
                m_lastMouseX = x;
                m_lastMouseY = y;
                m_hasLastMousePosition = true;
            }

            m_input.mouseDeltaX += static_cast<float>(x - m_lastMouseX);
            m_input.mouseDeltaY += static_cast<float>(y - m_lastMouseY);

            m_lastMouseX = x;
            m_lastMouseY = y;

            return 0;
        }
        case WM_SETFOCUS:
        {
            m_input.windowFocused = true;
            m_hasLastMousePosition = false;
            return 0;
        }

        case WM_KILLFOCUS:
        {
            m_input = {};
            m_input.windowFocused = false;
            m_hasLastMousePosition = false;
            return 0;
        }
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
}
