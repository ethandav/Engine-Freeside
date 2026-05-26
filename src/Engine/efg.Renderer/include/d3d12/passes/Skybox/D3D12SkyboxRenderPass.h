#pragma once
#include "..\..\frame\D3D12FrameContext.h"
#include "..\..\..\render\types\FramePacket.h"

namespace efg::d3d12
{

    class D3D12GraphicsPipelineLibrary;
    class D3D12DescriptorContext;

    class D3D12SkyboxRenderPass
    {
    public:
        void Initialize(D3D12GraphicsPipelineLibrary* graphicsPipelineLibrary, D3D12DescriptorContext* descriptorCtx);
        void Execute(const FrameContext& ctx, const FramePacket& scene);
    private:
        D3D12GraphicsPipelineLibrary* m_pipelineLibrary = nullptr;
        D3D12DescriptorContext* m_descriptorContext = nullptr;
    };
}