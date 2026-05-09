#pragma once

#include "Renderer.h" //TODO: Move needed types from here so I can stop including renderer header
#include "MeshData.h"

class IRendererBackend
{
public:
    virtual ~IRendererBackend() = default;
    virtual void Initialize(const RendererDesc& desc) = 0;
    virtual void Shutdown() = 0;
    virtual void Render(const FramePacket& sceneRenderData) = 0;
    virtual efg::MeshHandle CreateMesh(const efg::MeshData& mesh) = 0;
};