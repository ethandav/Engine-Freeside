#pragma once

#include "..\include\FormatUtils.h"
#include "..\include\Format.h"

#include <cstdint>

namespace Freeside
{
    uint32_t BytesPerPixelOrBlock(Format format);
}