#include "..\..\..\include\d3d12\core\D3D12RendererBackend.h"
#include "..\..\..\include\d3d12\core\D3D12Pix.h"

#if defined(EFG_ENABLE_IMGUI)
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

static efg::d3d12::D3D12DescriptorContext* g_imguiDescriptorContext = nullptr;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif

namespace efg::d3d12
{
    void D3D12RendererBackend::Initialize(const Freeside::RendererDesc& desc)
    {
        InitializeD3D12Systems(desc);
        CreateRenderTargets(desc.width, desc.height);
#if defined(EFG_ENABLE_IMGUI)
        InitializeImgui(desc.nativeWindowHandle);
        g_imguiDescriptorContext = &m_device.DescriptorContext();
#endif
    }

    void D3D12RendererBackend::CreateRenderTargets(uint32_t width, uint32_t height)
    {
        m_renderTargets.sceneDepth = m_resources.TextureFactory().CreateDepthBuffer(width, height, DescriptorVisibility::CpuOnly);

        GpuTexture2D sceneColor = {};
        sceneColor.resourceFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        sceneColor.rtvFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        sceneColor.flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        sceneColor.srvFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        sceneColor.initialState = D3D12_RESOURCE_STATE_PRESENT;
        sceneColor.width = width;
        sceneColor.height = height;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        clearValue.Color[0] = 0.0f;
        clearValue.Color[1] = 0.0f;
        clearValue.Color[2] = 0.0f;
        clearValue.Color[3] = 1.0f;

        m_resources.TextureFactory().CreateTexture2D(sceneColor, DescriptorVisibility::CpuOnlyAndShaderVisible, &clearValue);
        m_renderTargets.sceneColor = sceneColor;

        sceneColor.resource.Get()->SetName(L"HDR Scene Color Render Target");
        m_renderTargets.sceneDepth.resource.Get()->SetName(L"Scene Depth Buffer");
    }

    void D3D12RendererBackend::InitializeD3D12Systems(const Freeside::RendererDesc& desc)
    {
        m_device.Initialize(desc);
        m_resources.Initialize(&m_device);
        m_pipeline.Initialize(m_device.GraphicsContext().GetDevice());
        m_frame.Initialize(&m_device, &m_resources, desc.width, desc.height);
        m_passes.Initialize(&m_device, &m_resources, &m_pipeline, &m_frame, &m_renderTargets);
    }

#if defined(EFG_ENABLE_IMGUI)
    void D3D12RendererBackend::InitializeImgui(void* hwnd)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        // Setup Platform/Renderer backends
        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = m_device.GraphicsContext().GetDevice();
        init_info.CommandQueue = m_device.DirectCommandContext().GetDirectCommandQueue();;
        init_info.NumFramesInFlight = NumFramesInFlight;
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // Or your render target format.

        // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
        // The example_win32_directx12/main.cpp application include a simple free-list based allocator.
        init_info.SrvDescriptorHeap = m_device.DescriptorContext().GetCBVSRVUAVHeap();
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
            { 
                GpuDescriptorAllocation allocation = g_imguiDescriptorContext->AllocateShaderVisibleCBVSRVUAV();

                *out_cpu_handle = allocation.cpu;
                *out_gpu_handle = allocation.gpu;
            };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
            {
                return;
            };

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX12_Init(&init_info);
    }

    bool D3D12RendererBackend::HandleImguiWindowMessage(void* hwnd, uint32_t message, uintptr_t wParam, intptr_t lParam, intptr_t& outResult)
    {
        LRESULT result = ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(hwnd), static_cast<UINT>(message), static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));

        if (result != 0)
        {
            outResult = static_cast<intptr_t>(result);
            return true;
        }

        return false;
    }

    void D3D12RendererBackend::BeginImguiFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void D3D12RendererBackend::RenderImguiFrame()
    {
        ImGui::Render();
        auto* commandList = m_device.DirectCommandContext().GetDirectCommandList();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
    }
#endif

    void D3D12RendererBackend::Shutdown()
    {
        m_device.DirectFence().WaitForGPU(m_device.DirectCommandContext().GetDirectCommandQueue());
        m_frame.Shutdown();
#if defined(EFG_ENABLE_IMGUI)
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
#endif
    }

    void D3D12RendererBackend::Render(const FramePacket& scene)
    {
        FrameContext frameCtx = m_frame.BeginFrame(&m_renderQueue);
        m_passes.Execute(scene, frameCtx, m_renderQueue);
#if defined(EFG_ENABLE_IMGUI)
        BeginImguiFrame();
        ImGui::ShowDemoWindow();
        RenderImguiFrame();
#endif
        m_frame.EndFrame(frameCtx, m_renderTargets);
    }

    Freeside::MeshHandle D3D12RendererBackend::CreateMesh(const Freeside::MeshDesc& mesh)
    {
        return m_resources.CreateMesh(mesh);
    }

    Freeside::MaterialHandle D3D12RendererBackend::RegisterMaterial(const Freeside::MaterialDesc& mat)
    {
        return m_resources.RegisterMaterial(mat);
    }

    Freeside::TextureHandle D3D12RendererBackend::CreateMaterialTexture2d(const Freeside::TextureDesc& texture)
    {
        return m_resources.CreateMaterialTexture2d(texture);
    }

    Freeside::TextureHandle D3D12RendererBackend::CreateTextureCube(const std::array<Freeside::TextureDesc, 6> faces)
    {
        return m_resources.CreateTextureCube(faces);
    }
}