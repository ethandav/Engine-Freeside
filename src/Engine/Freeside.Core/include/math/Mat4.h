#pragma once

#include <cmath>

#include "Vec3.h"
#include "Vec4.h"
#include "Quat.h"

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
    }
}