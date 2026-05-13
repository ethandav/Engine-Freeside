#pragma once

#include "RenderTypes.h"

namespace efg
{
    class IRendererBackend
    {
    public:
        virtual ~IRendererBackend() = default;
        virtual void Initialize(const RendererDesc& desc) = 0;
        virtual void Shutdown() = 0;
        virtual void Render(const FramePacket& sceneRenderData) = 0;
        virtual MeshHandle CreateMesh(const MeshData& mesh) = 0;
        virtual MaterialHandle RegisterMaterial(const MaterialDesc& mat) = 0;
        virtual TextureHandle RegisterTexture2D(const wchar_t* filename) = 0;

    };
}