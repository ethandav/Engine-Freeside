#pragma once
#include "..\..\frame\D3D12FrameContext.h"
#include "..\..\..\render\types\FramePacket.h"
#include "..\..\resources\D3D12GpuMesh.h"

namespace efg::d3d12
{

    class D3D12GraphicsPipelineLibrary;
    class D3D12DescriptorContext;
    class D3D12MaterialTextureLibrary;
    class D3D12BufferFactory;

    class D3D12SkyboxRenderPass
    {
    public:
        void Initialize(D3D12GraphicsPipelineLibrary* graphicsPipelineLibrary, D3D12DescriptorContext* descriptorCtx, D3D12MaterialTextureLibrary* textureLibrary, D3D12BufferFactory* bufferFactory);
        void Execute(const FrameContext& ctx, const FramePacket& scene);
        void SetSkyboxMesh(GpuMesh mesh);
    private:
        D3D12GraphicsPipelineLibrary* m_pipelineLibrary = nullptr;
        D3D12DescriptorContext* m_descriptorContext = nullptr;
        D3D12MaterialTextureLibrary* m_textureLibrary = nullptr;
        D3D12BufferFactory* m_bufferFactory  = nullptr;

        GpuMesh m_skyboxMesh = {};
    };
}