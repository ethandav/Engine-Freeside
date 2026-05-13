#pragma once

#include <dxgiformat.h>
#include "..\render\types\Format.h"

namespace efg::d3d12
{
    DXGI_FORMAT ToDxgiFormat(efg::Format format);
}