#include "..\..\..\include\d3d12\core\D3D12Context.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"


namespace efg::d3d12
{
    void D3D12Context::CreateFactory()
    {
        UINT dxgiFactoryFlags = 0;
        D3D12_THROW_IF_FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));
    }

    void D3D12Context::CreateDevice(bool useWarpDevice)
    {
        if (useWarpDevice)
        {
            ComPtr<IDXGIAdapter> warpAdapter;

            m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
            D3D12_THROW_IF_FAILED(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
        }
        else
        {
            GetHardwareAdapter();
            D3D12_THROW_IF_FAILED(D3D12CreateDevice(m_hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
        }
    }

    void D3D12Context::GetHardwareAdapter(bool requestHighPerformanceAdapter)
    {
        m_hardwareAdapter.Reset();

        ComPtr<IDXGIFactory6> factory6;

        if (SUCCEEDED(m_factory.As(&factory6)))
        {
            for (UINT adapterIndex = 0; ; ++adapterIndex)
            {
                ComPtr<IDXGIAdapter1> adapter;
                DXGI_ADAPTER_DESC1 desc = {};

                D3D12_THROW_IF_FAILED(factory6->EnumAdapterByGpuPreference(adapterIndex, requestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter)));
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    m_hardwareAdapter = adapter;
                    return;
                }
            }
        }

        for (UINT adapterIndex = 0; ; ++adapterIndex)
        {
            ComPtr<IDXGIAdapter1> adapter;
            DXGI_ADAPTER_DESC1 desc = {};

            D3D12_THROW_IF_FAILED(m_factory->EnumAdapters1(adapterIndex, &adapter));
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                m_hardwareAdapter = adapter;
                return;
            }
        }
    }
}