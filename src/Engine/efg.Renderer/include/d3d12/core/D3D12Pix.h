#pragma once
#include <cstdint>
#if defined(EFG_ENABLE_PIX)
#include <pix3.h>
namespace efg::d3d12::PixColors
{
    inline const std::uint64_t ShadowMapPass = PIX_COLOR(155, 95, 215);
    inline const std::uint64_t ForwardLitPass = PIX_COLOR(30, 255, 70);
    inline const std::uint64_t BackbufferSetup = PIX_COLOR(25, 95, 255);
}
#else
#define PIXBeginEvent(...)
#define PIXEndEvent(...)
#define PIXSetMarker(...)
#define PIX_COLOR(r, g, b) 0
#endif