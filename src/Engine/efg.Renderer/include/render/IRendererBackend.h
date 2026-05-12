#pragma once

#include "RenderTypes.h"

class IRendererBackend
{
public:
    virtual ~IRendererBackend() = default;
    virtual void Initialize(const efg::RendererDesc& desc) = 0;
    virtual void Shutdown() = 0;
    virtual void Render(const efg::FramePacket& sceneRenderData) = 0;
    virtual efg::MeshHandle CreateMesh(const efg::MeshData& mesh) = 0;
    virtual efg::MaterialHandle RegisterMaterial(const efg::MaterialDesc& mat) = 0;

};