#pragma once

#include "Renderer.h" //TODO: Move needed types from here so I can stop including renderer header
#include "MeshData.h"

class IRendererBackend
{
public:
    virtual ~IRendererBackend() = default;
    virtual void Initialize(const RendererDesc& desc) = 0;
    virtual void Shutdown() = 0;
    virtual void BeginFrame(efg::Camera camera) = 0;
    virtual void EndFrame() = 0;
    virtual efg::MeshHandle CreateMesh(const efg::MeshData& mesh) = 0;
    virtual void DrawMesh(efg::MeshHandle handle) = 0;
};