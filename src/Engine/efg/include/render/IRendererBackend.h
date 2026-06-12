#pragma once

#include "types\RendererDesc.h"
#include "types\FramePacket.h"
#include "types\MeshTypes.h"
#include "types\Handles.h"

#include "..\..\..\Freeside.Assets\include\MaterialTypes.h"

namespace efg::d3d12
{
    class IRendererBackend
    {
    public:
        virtual ~IRendererBackend() = default;
        virtual void Initialize(const Freeside::RendererDesc& desc) = 0;
        virtual void Shutdown() = 0;
        virtual void Render(const FramePacket& sceneRenderData) = 0;
        virtual Freeside::MeshHandle CreateMesh(const Freeside::MeshDesc& mesh) = 0;
        virtual Freeside::MaterialHandle RegisterMaterial(const Freeside::MaterialDesc& mat) = 0;
        virtual Freeside::TextureHandle CreateMaterialTexture2d(const Freeside::TextureDesc& texture) = 0;
        virtual Freeside::TextureHandle CreateTextureCube(const std::array<Freeside::TextureDesc, 6> faces) = 0;
#if defined(EFG_ENABLE_IMGUI)
        virtual bool HandleImguiWindowMessage(void* hwnd, uint32_t message, uintptr_t wParam, intptr_t lParam, intptr_t& outResult) = 0;
#endif
    };
}