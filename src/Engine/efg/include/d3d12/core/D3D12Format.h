#pragma once

#include "..\..\render\types\Format.h"

#include <dxgiformat.h>

namespace efg::d3d12
{
    DXGI_FORMAT ToDxgiFormat(Format format);
}