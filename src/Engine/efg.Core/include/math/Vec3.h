#pragma once

namespace Engine
{
    struct Vec3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        constexpr Vec3() = default;

        constexpr Vec3(float xValue, float yValue, float zValue)
            : x(xValue), y(yValue), z(zValue)
        {
        }
    };
}