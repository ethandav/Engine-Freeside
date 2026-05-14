#pragma once

#if defined(EFG_ENABLE_PIX)
#include <pix3.h>
#else
#define PIXBeginEvent(...)
#define PIXEndEvent(...)
#define PIXSetMarker(...)
#define PIX_COLOR(r, g, b) 0
#endif