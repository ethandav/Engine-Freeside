#pragma once
#pragma once
#include <array>
#include <cstdint>

namespace Freeside
{
    enum class Key : uint16_t
    {
        Unknown = 0,

        W,
        A,
        S,
        D,

        Q,
        E,

        Space,
        Escape,
        LeftShift,
        LeftCtrl,

        Count
    };

    enum class MouseButton : uint8_t
    {
        Left = 0,
        Right,
        Middle,

        Count
    };

    struct InputState
    {
        std::array<bool, static_cast<size_t>(Key::Count)> keysDown{};
        std::array<bool, static_cast<size_t>(MouseButton::Count)> mouseButtonsDown{};

        float mouseDeltaX = 0.0f;
        float mouseDeltaY = 0.0f;

        int mouseX = 0;
        int mouseY = 0;

        bool windowFocused = true;
        bool cursorLocked = false;

        bool IsKeyDown(Key key) const
        {
            return keysDown[static_cast<size_t>(key)];
        }

        bool IsMouseButtonDown(MouseButton button) const
        {
            return mouseButtonsDown[static_cast<size_t>(button)];
        }
    };
}