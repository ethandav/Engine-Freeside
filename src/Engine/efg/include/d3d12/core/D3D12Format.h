#pragma once

#include "..\..\..\..\Freeside.Assets\include\Format.h"

#include <dxgiformat.h>

namespace efg::d3d12
{
    DXGI_FORMAT ToDxgiFormat(Freeside::Format format);
}