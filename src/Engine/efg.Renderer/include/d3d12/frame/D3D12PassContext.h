#pragma once

#include "..\..\render\RenderQueue.h"
#include "..\frame\D3D12FrameContext.h"
#include "..\types\D3D12RenderDeviceServices.h"
#include "..\types\D3D12RenderSceneResources.h"

namespace efg::d3d12
{
    struct D3D12PassContext
    {
        FrameContext* frameContext = nullptr;
        D3D12RenderServices* services = nullptr;
        D3D12RenderResourcesLibraries* libraries = nullptr;
        //D3D12RenderTargetManager* targets = nullptr;
        RenderQueue* renderQueue = nullptr;
    };
}