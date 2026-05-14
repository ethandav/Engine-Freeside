#include "..\..\..\include\d3d12\core\D3D12Context.h"

namespace efg::d3d12
{
    void D3D12Context::Initialize(bool useWarpDevice)
    {
        UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();

                dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }
#endif
        CreateFactory();
        CreateDevice(useWarpDevice);
    }

    ID3D12Device* D3D12Context::GetDevice()
    {
        return m_device.Get();
    }

    IDXGIFactory4* D3D12Context::GetFactory()
    {
        return m_factory.Get();
    }
}