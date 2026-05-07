#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"

namespace efg
{
	namespace Math
	{
		constexpr float PI = 3.141592654f;
        constexpr float TwoPI = 6.283185307f;
        constexpr float PI_DIV_2 = 1.570796327f;
        constexpr float OneDIVPI = 0.318309886f;
        constexpr float OneDIV2PI = 0.159154943f;
        constexpr float PIDIV2 = 1.570796327f;
        constexpr float PIDIV4 = 0.785398163f;

        inline void ScalarSinCos(float* pSin,float* pCos,float  Value)
        {
            float quotient = OneDIV2PI * Value;
            if (Value >= 0.0f)
            {
                quotient = static_cast<float>(static_cast<int>(quotient + 0.5f));
            }
            else
            {
                quotient = static_cast<float>(static_cast<int>(quotient - 0.5f));
            }
            float y = Value - TwoPI * quotient;

            float sign;
            if (y > PIDIV2)
            {
                y = PI - y;
                sign = -1.0f;
            }
            else if (y < -PIDIV2)
            {
                y = -PI - y;
                sign = -1.0f;
            }
            else
            {
                sign = +1.0f;
            }

            float y2 = y * y;

            *pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

            float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
            *pCos = sign * p;
        }
	}
}