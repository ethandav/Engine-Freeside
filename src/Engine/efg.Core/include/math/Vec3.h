#pragma once

#include <cmath>

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

        inline Vec3 operator-(const Vec3& a, const Vec3& b)
    {
        return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    inline Vec3 operator+(const Vec3& a, const Vec3& b)
    {
        return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    inline Vec3 operator*(const Vec3& v, float scalar)
    {
        return Vec3(v.x * scalar, v.y * scalar, v.z * scalar);
    }

    inline float Dot(const Vec3& a, const Vec3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline Vec3 Cross(const Vec3& a, const Vec3& b)
    {
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    inline Vec3 Normalize(const Vec3& v)
    {
        const float length = std::sqrt(Dot(v, v));

        if (length == 0.0f)
        {
            return Vec3();
        }

        return Vec3(v.x / length, v.y / length, v.z / length);
    }
}