#pragma once

#include "..\resources\D3D12GpuTexture.h"
#include "..\..\..\..\Freeside.Core\include\math\Mat4.h"
#include <cstdint>
#include <vector>

namespace efg::d3d12
{
    static constexpr uint32_t MaxDirectionalShadows = 16;
    static constexpr uint32_t MaxPointShadows = 64;

    struct DirectionalShadowEntry
    {
        uint32_t lightIndex = 0;
        GpuTexture2D* shadowMap = nullptr;
        Freeside::Math::Mat4 lightViewProjection = {};
    };

    struct PointShadowEntry
    {
        uint32_t lightIndex = 0;
        GpuPointShadowCube* shadowCube = nullptr;
        Freeside::Math::Mat4 faceViewProjection[6] = {};
        float farPlane = 0.0f;
    };

    struct ShadowMapFrameData
    {
        std::vector<DirectionalShadowEntry> directionalShadows;
        std::vector<PointShadowEntry> pointShadows;
    };
}