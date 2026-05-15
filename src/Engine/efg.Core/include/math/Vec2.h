#pragma once

namespace Freeside
{
    namespace Math
    {
        struct Vec2
        {
            float x = 0.0f;
            float y = 0.0f;

            constexpr Vec2() = default;

            constexpr Vec2(float xValue, float yValue)
                : x(xValue), y(yValue)
            {
            }
        };
    }
}