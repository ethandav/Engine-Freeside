#pragma once
#include "..\..\efg.Core\include\math\Vec2.h"
#include "..\..\efg.Core\include\math\Vec3.h"
#include "..\..\efg.Core\include\math\Vec4.h"
#include <cstdint>

namespace efg
{
    struct MaterialHandle
    {
        uint32_t index = UINT32_MAX;

        bool IsValid() const
        {
            return index != UINT32_MAX;
        }
    };

    struct MaterialDesc
    {
        Vec3 baseColor = Vec3(0.8f, 0.8f, 0.8f);
        Vec2 specular = Vec2(1.0f, 64.0f);
    };
}