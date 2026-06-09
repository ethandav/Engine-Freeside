#pragma once

#include "Vec3.h"
#include <cmath>

namespace Freeside
{
    namespace Math
    {
        struct Quat
        {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            float w = 1.0f;

            constexpr Quat() = default;

            constexpr Quat(float xValue, float yValue, float zValue, float wValue)
                : x(xValue), y(yValue), z(zValue), w(wValue)
            {
            }

            static constexpr Quat Identity()
            {
                return Quat(0.0f, 0.0f, 0.0f, 1.0f);
            }
        };

        inline float Dot(const Quat& a, const Quat& b)
        {
            return a.x * b.x +
                a.y * b.y +
                a.z * b.z +
                a.w * b.w;
        }

        inline float LengthSquared(const Quat& q)
        {
            return Dot(q, q);
        }

        inline float Length(const Quat& q)
        {
            return std::sqrt(LengthSquared(q));
        }

        inline Quat Normalize(const Quat& q)
        {
            const float length = Length(q);

            if (length == 0.0f)
            {
                return Quat::Identity();
            }

            return Quat(
                q.x / length,
                q.y / length,
                q.z / length,
                q.w / length
            );
        }

        inline Quat Conjugate(const Quat& q)
        {
            return Quat(-q.x, -q.y, -q.z, q.w);
        }

        inline Quat Inverse(const Quat& q)
        {
            const float lengthSq = LengthSquared(q);

            if (lengthSq == 0.0f)
            {
                return Quat::Identity();
            }

            const Quat conjugate = Conjugate(q);

            return Quat(
                conjugate.x / lengthSq,
                conjugate.y / lengthSq,
                conjugate.z / lengthSq,
                conjugate.w / lengthSq
            );
        }

        inline Quat operator*(const Quat& a, const Quat& b)
        {
            return Quat(
                a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
                a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
            );
        }

        inline Quat& operator*=(Quat& a, const Quat& b)
        {
            a = a * b;
            return a;
        }

        inline Quat FromAxisAngle(const Vec3& axis, float angleRadians)
        {
            const Vec3 normalizedAxis = Normalize(axis);

            const float halfAngle = angleRadians * 0.5f;
            const float s = std::sin(halfAngle);
            const float c = std::cos(halfAngle);

            return Normalize(Quat(
                normalizedAxis.x * s,
                normalizedAxis.y * s,
                normalizedAxis.z * s,
                c
            ));
        }

        inline Quat FromEulerXYZ(float xRadians, float yRadians, float zRadians)
        {
            const float halfX = xRadians * 0.5f;
            const float halfY = yRadians * 0.5f;
            const float halfZ = zRadians * 0.5f;

            const float sx = std::sin(halfX);
            const float cx = std::cos(halfX);

            const float sy = std::sin(halfY);
            const float cy = std::cos(halfY);

            const float sz = std::sin(halfZ);
            const float cz = std::cos(halfZ);

            Quat qx(sx, 0.0f, 0.0f, cx);
            Quat qy(0.0f, sy, 0.0f, cy);
            Quat qz(0.0f, 0.0f, sz, cz);

            return Normalize(qz * qy * qx);
        }

        inline Quat FromEulerXYZ(const Vec3& radians)
        {
            return FromEulerXYZ(radians.x, radians.y, radians.z);
        }

        inline Vec3 RotateVector(const Quat& rotation, const Vec3& vector)
        {
            const Quat q = Normalize(rotation);

            const Quat v(vector.x, vector.y, vector.z, 0.0f);
            const Quat result = q * v * Inverse(q);

            return Vec3(result.x, result.y, result.z);
        }
    }
}