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

        inline Vec2 operator-(const Vec2& a, const Vec2& b)
        {
            return Vec2(a.x - b.x, a.y - b.y);
        }
    }
}