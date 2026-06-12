#include "..\include\Window.h"

#include <windowsx.h>
#include <stdexcept>
#include <vector>
#include <cstddef>

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

        RegisterRawMouseInput();
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
        std::fill(m_input.keysPressed.begin(), m_input.keysPressed.end(), false);
        std::fill(m_input.keysReleased.begin(), m_input.keysReleased.end(), false);

        std::fill(m_input.mouseButtonsPressed.begin(), m_input.mouseButtonsPressed.end(), false);
        std::fill(m_input.mouseButtonsReleased.begin(), m_input.mouseButtonsReleased.end(), false);

        m_input.mouseDeltaX = 0.0f;
        m_input.mouseDeltaY = 0.0f;

        PollEvents();

        return m_input;
    }

    void Window::SetCursorLocked(bool locked)
    {
        if (m_cursorLocked == locked)
            return;

        m_cursorLocked = locked;

        if (m_cursorLocked)
        {
            UpdateCursorClip();

            while (ShowCursor(FALSE) >= 0)
            {
            }
        }
        else
        {
            ClipCursor(nullptr);

            while (ShowCursor(TRUE) < 0)
            {
            }
        }
    }

    void Window::SetNativeMessageHandler(NativeMessageHandler handler)
    {
        m_nativeMessageHandler = std::move(handler);
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
        if (m_nativeMessageHandler)
        {
            LRESULT result = 0;

            if (m_nativeMessageHandler(m_hwnd, message, wParam, lParam, result))
            {
                return result;
            }
        }

        switch (message)
        {
        case WM_CLOSE:
        {
            m_isOpen = false;
            DestroyWindow(m_hwnd);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_KILLFOCUS:
        {
            SetCursorLocked(false);

            m_input = {};
            m_input.windowFocused = false;

            return 0;
        }
        case WM_MOVE:
        case WM_SIZE:
        {
            UpdateCursorClip();
            return 0;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            Freeside::Key key = TranslateVirtualKey(wParam);
            if (key != Freeside::Key::Unknown)
            {
                size_t index = static_cast<size_t>(key);

                if (!m_input.keysDown[index])
                {
                    m_input.keysPressed[index] = true;
                }

                m_input.keysDown[index] = true;
            }
            return 0;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            Freeside::Key key = TranslateVirtualKey(wParam);
            if (key != Freeside::Key::Unknown)
            {
                size_t index = static_cast<size_t>(key);

                m_input.keysDown[index] = false;
                m_input.keysReleased[index] = true;
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

            return 0;
        }
        case WM_SETFOCUS:
        {
            m_input.windowFocused = true;
            return 0;
        }
        case WM_INPUT:
        {
            UINT size = 0;

            GetRawInputData(
                reinterpret_cast<HRAWINPUT>(lParam),
                RID_INPUT,
                nullptr,
                &size,
                sizeof(RAWINPUTHEADER)
            );

            if (size == 0)
                return 0;

            std::vector<std::byte> buffer(size);

            UINT bytesRead = GetRawInputData(
                reinterpret_cast<HRAWINPUT>(lParam),
                RID_INPUT,
                buffer.data(),
                &size,
                sizeof(RAWINPUTHEADER)
            );

            if (bytesRead != size)
                return 0;

            const RAWINPUT* raw = reinterpret_cast<const RAWINPUT*>(buffer.data());

            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                const RAWMOUSE& mouse = raw->data.mouse;

                if (m_cursorLocked && mouse.usFlags == MOUSE_MOVE_RELATIVE)
                {
                    m_input.mouseDeltaX += static_cast<float>(mouse.lLastX);
                    m_input.mouseDeltaY += static_cast<float>(mouse.lLastY);
                }
            }

            return 0;
        }
        }

        return DefWindowProc(m_hwnd, message, wParam, lParam);
    }

    void Window::UpdateCursorClip()
    {
        if (!m_cursorLocked)
            return;

        RECT clientRect = {};
        GetClientRect(m_hwnd, &clientRect);

        POINT upperLeft = { clientRect.left, clientRect.top };
        POINT lowerRight = { clientRect.right, clientRect.bottom };

        ClientToScreen(m_hwnd, &upperLeft);
        ClientToScreen(m_hwnd, &lowerRight);

        RECT clipRect = {};
        clipRect.left = upperLeft.x;
        clipRect.top = upperLeft.y;
        clipRect.right = lowerRight.x;
        clipRect.bottom = lowerRight.y;

        ClipCursor(&clipRect);
    }

    void Window::RegisterRawMouseInput()
    {
        RAWINPUTDEVICE rid = {};
        rid.usUsagePage = 0x01; // Generic Desktop Controls
        rid.usUsage = 0x02;     // Mouse
        rid.dwFlags = 0;
        rid.hwndTarget = m_hwnd;

        if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
        {
            throw std::runtime_error("Failed to register raw mouse input.");
        }
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
