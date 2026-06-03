#pragma once

#include <cmath>

#include "Vec3.h"
#include "Vec4.h"

namespace Freeside
{
    namespace Math
    {
        struct Mat4
        {
            // Row-major storage:
            //
            // [ m00 m01 m02 m03 ]
            // [ m10 m11 m12 m13 ]
            // [ m20 m21 m22 m23 ]
            // [ m30 m31 m32 m33 ]
            float m[4][4] = {};

            constexpr Mat4() = default;

            constexpr Mat4(
                float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13,
                float m20, float m21, float m22, float m23,
                float m30, float m31, float m32, float m33)
                : m{
                    { m00, m01, m02, m03 },
                    { m10, m11, m12, m13 },
                    { m20, m21, m22, m23 },
                    { m30, m31, m32, m33 }
                }
            {
            }

            static constexpr Mat4 Identity()
            {
                return Mat4(
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
                );
            }

            static constexpr Mat4 Zero()
            {
                return Mat4();
            }

            float* Data()
            {
                return &m[0][0];
            }

            const float* Data() const
            {
                return &m[0][0];
            }

            constexpr float& operator()(int row, int column)
            {
                return m[row][column];
            }

            constexpr const float& operator()(int row, int column) const
            {
                return m[row][column];
            }
        };

        inline Mat4 operator*(const Mat4& a, const Mat4& b)
        {
            Mat4 result = Mat4::Zero();

            for (int row = 0; row < 4; row++)
            {
                for (int column = 0; column < 4; column++)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        result(row, column) += a(row, i) * b(i, column);
                    }
                }
            }

            return result;
        }

        inline Vec4 operator*(const Mat4& matrix, const Vec4& vector)
        {
            return Vec4(
                matrix(0, 0) * vector.x + matrix(0, 1) * vector.y + matrix(0, 2) * vector.z + matrix(0, 3) * vector.w,
                matrix(1, 0) * vector.x + matrix(1, 1) * vector.y + matrix(1, 2) * vector.z + matrix(1, 3) * vector.w,
                matrix(2, 0) * vector.x + matrix(2, 1) * vector.y + matrix(2, 2) * vector.z + matrix(2, 3) * vector.w,
                matrix(3, 0) * vector.x + matrix(3, 1) * vector.y + matrix(3, 2) * vector.z + matrix(3, 3) * vector.w
            );
        }

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

        inline Mat4 TransformMatrix(const Vec3 position, const Vec3 rotation, const Vec3 scale)
        {
            Mat4 scaleMatrix = Scale(scale.x, scale.y, scale.z);
            Mat4 rotationMatrix = RotationZ(rotation.z) * RotationX(rotation.x) * RotationY(rotation.y);
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
    }
}