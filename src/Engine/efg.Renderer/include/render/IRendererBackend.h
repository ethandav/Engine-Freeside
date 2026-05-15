#pragma once

#include "types/RendererDesc.h"
#include "types/FramePacket.h"
#include "types/MeshTypes.h"
#include "types/MaterialTypes.h"
#include "types/Handles.h"

namespace efg::d3d12
{
    class IRendererBackend
    {
    public:
        virtual ~IRendererBackend() = default;
        virtual void Initialize(const Freeside::RendererDesc& desc) = 0;
        virtual void Shutdown() = 0;
        virtual void Render(const FramePacket& sceneRenderData) = 0;
        virtual Freeside::MeshHandle CreateMesh(const Freeside::MeshData& mesh) = 0;
        virtual Freeside::MaterialHandle RegisterMaterial(const Freeside::MaterialDesc& mat) = 0;
        virtual Freeside::TextureHandle RegisterTexture2D(const wchar_t* filename) = 0;

    };
}