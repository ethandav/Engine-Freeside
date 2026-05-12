#pragma once
#include "..\..\..\efg.Core\include\math\Vec2.h"
#include "..\..\..\efg.Core\include\math\Vec3.h"
#include "..\..\..\efg.Core\include\math\Vec4.h"
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

        bool operator==(const MaterialHandle& other) const
        {
            return index == other.index;
        }

        bool operator!=(const MaterialHandle& other) const
        {
            return !(*this == other);
        }
    };

    struct MaterialDesc
    {
        Math::Vec3 baseColor = Math::Vec3(0.8f, 0.8f, 0.8f);
        Math::Vec2 specular = Math::Vec2(1.0f, 64.0f);
    };
}