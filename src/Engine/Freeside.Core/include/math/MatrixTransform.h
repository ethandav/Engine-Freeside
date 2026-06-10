#pragma once

#include "Mat4.h"
#include "Quat.h"

namespace Freeside::Math
{
    inline Mat4 Transpose(const Mat4& matrix)
    {
        return Mat4(
            matrix(0, 0), matrix(1, 0), matrix(2, 0), matrix(3, 0),
            matrix(0, 1), matrix(1, 1), matrix(2, 1), matrix(3, 1),
            matrix(0, 2), matrix(1, 2), matrix(2, 2), matrix(3, 2),
            matrix(0, 3), matrix(1, 3), matrix(2, 3), matrix(3, 3)
        );
    }

    inline Mat4 Translation(float x, float y, float z)
    {
        return Mat4(
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4 Scale(float x, float y, float z)
    {
        return Mat4(
            x, 0.0f, 0.0f, 0.0f,
            0.0f, y, 0.0f, 0.0f,
            0.0f, 0.0f, z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4 RotationZ(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return Mat4(
            c, -s, 0.0f, 0.0f,
            s, c, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4 RotationY(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return Mat4(
            c, 0.0f, -s, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            s, 0.0f, c, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4 RotationX(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return Mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, c, -s, 0.0f,
            0.0f, s, c, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4 Mat4FromQuat(const Quat& rotation)
    {
        const Quat q = Normalize(rotation);

        const float x = q.x;
        const float y = q.y;
        const float z = q.z;
        const float w = q.w;

        const float xx = x * x;
        const float yy = y * y;
        const float zz = z * z;

        const float xy = x * y;
        const float xz = x * z;
        const float yz = y * z;

        const float wx = w * x;
        const float wy = w * y;
        const float wz = w * z;

        Mat4 result = Mat4::Identity();

        result.m[0][0] = 1.0f - 2.0f * (yy + zz);
        result.m[0][1] = 2.0f * (xy - wz);
        result.m[0][2] = 2.0f * (xz + wy);
        result.m[0][3] = 0.0f;

        result.m[1][0] = 2.0f * (xy + wz);
        result.m[1][1] = 1.0f - 2.0f * (xx + zz);
        result.m[1][2] = 2.0f * (yz - wx);
        result.m[1][3] = 0.0f;

        result.m[2][0] = 2.0f * (xz - wy);
        result.m[2][1] = 2.0f * (yz + wx);
        result.m[2][2] = 1.0f - 2.0f * (xx + yy);
        result.m[2][3] = 0.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    inline Mat4 TransformMatrix(const Vec3 position, const Quat rotation, const Vec3 scale)
    {
        Mat4 scaleMatrix = Scale(scale.x, scale.y, scale.z);
        Mat4 rotationMatrix = Mat4FromQuat(rotation);
        Mat4 translationMatrix = Translation(position.x, position.y, position.z);

        return translationMatrix * rotationMatrix * scaleMatrix;
    }

    inline Mat4 LookAtLH(const Vec3& eye, const Vec3& target, const Vec3& up)
    {
        const Vec3 zAxis = Normalize(target - eye);        // forward
        const Vec3 xAxis = Normalize(Cross(up, zAxis));    // right
        const Vec3 yAxis = Cross(zAxis, xAxis);            // up

        return Mat4(
            xAxis.x, xAxis.y, xAxis.z, -Dot(xAxis, eye),
            yAxis.x, yAxis.y, yAxis.z, -Dot(yAxis, eye),
            zAxis.x, zAxis.y, zAxis.z, -Dot(zAxis, eye),
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    inline Mat4 PerspectiveFovLH(float fovYRadians, float aspectRatio, float nearZ, float farZ)
    {
        const float yScale = 1.0f / std::tan(fovYRadians * 0.5f);
        const float xScale = yScale / aspectRatio;

        return Mat4(
            xScale, 0.0f, 0.0f, 0.0f,
            0.0f, yScale, 0.0f, 0.0f,
            0.0f, 0.0f, farZ / (farZ - nearZ), (-nearZ * farZ) / (farZ - nearZ),
            0.0f, 0.0f, 1.0f, 0.0f
        );
    }

    inline Mat4 OrthographicLH(float width, float height, float nearZ, float farZ)
    {
        return Mat4(
            2.0f / width, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / height, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f / (farZ - nearZ), -nearZ / (farZ - nearZ),
            0.0f, 0.0f, 0.0f, 1.0f
        );
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

    inline Quat FromRotationMatrix(const Mat4& m)
    {
        const float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

        Quat q;

        if (trace > 0.0f)
        {
            const float s = std::sqrt(trace + 1.0f) * 2.0f;
            q.w = 0.25f * s;
            q.x = (m.m[2][1] - m.m[1][2]) / s;
            q.y = (m.m[0][2] - m.m[2][0]) / s;
            q.z = (m.m[1][0] - m.m[0][1]) / s;
        }
        else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2])
        {
            const float s = std::sqrt(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f;
            q.w = (m.m[2][1] - m.m[1][2]) / s;
            q.x = 0.25f * s;
            q.y = (m.m[0][1] + m.m[1][0]) / s;
            q.z = (m.m[0][2] + m.m[2][0]) / s;
        }
        else if (m.m[1][1] > m.m[2][2])
        {
            const float s = std::sqrt(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f;
            q.w = (m.m[0][2] - m.m[2][0]) / s;
            q.x = (m.m[0][1] + m.m[1][0]) / s;
            q.y = 0.25f * s;
            q.z = (m.m[1][2] + m.m[2][1]) / s;
        }
        else
        {
            const float s = std::sqrt(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f;
            q.w = (m.m[1][0] - m.m[0][1]) / s;
            q.x = (m.m[0][2] + m.m[2][0]) / s;
            q.y = (m.m[1][2] + m.m[2][1]) / s;
            q.z = 0.25f * s;
        }

        return Normalize(q);
    }

    inline Quat RotationLookingAt(const Vec3& position, const Vec3& target, const Vec3& worldUp = Vec3(0.0f, 1.0f, 0.0f))
    {
        Vec3 forward = Normalize(target - position);

        if (LengthSquared(forward) == 0.0f)
        {
            return Quat::Identity();
        }

        Vec3 right = Normalize(Cross(worldUp, forward));

        if (LengthSquared(right) == 0.0f)
        {
            return Quat::Identity();
        }

        Vec3 up = Cross(forward, right);

        Mat4 rotation = Mat4::Identity();

        rotation.m[0][0] = right.x;
        rotation.m[0][1] = right.y;
        rotation.m[0][2] = right.z;

        rotation.m[1][0] = up.x;
        rotation.m[1][1] = up.y;
        rotation.m[1][2] = up.z;

        rotation.m[2][0] = forward.x;
        rotation.m[2][1] = forward.y;
        rotation.m[2][2] = forward.z;

        return FromRotationMatrix(rotation);
    }
}