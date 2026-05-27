#pragma once
#include "..\..\frame\D3D12FrameContext.h"
#include "..\..\..\render\types\FramePacket.h"
#include "..\..\resources\D3D12GpuMesh.h"
#include "..\..\frame\D3D12PassContext.h"

namespace efg::d3d12
{
    class D3D12SkyboxRenderPass
    {
    public:
        void Execute(D3D12PassContext& ctx, const FramePacket& scene);
        void SetSkyboxMesh(GpuMesh mesh);
    private:
        GpuMesh m_skyboxMesh = {};
    };
}