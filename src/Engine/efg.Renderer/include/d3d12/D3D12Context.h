#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace efg::d3d12
{
	class D3D12Context
	{
	public:
		void Initialize(bool useWarpDevice);
		ID3D12Device* GetDevice();
		IDXGIFactory4* GetFactory();

	private:
		void CreateFactory();
		void GetHardwareAdapter(bool requestHighPerformanceAdapter = false);
		void CreateDevice(bool useWarpDevice);

		ComPtr<IDXGIFactory4> m_factory;
		ComPtr<IDXGIAdapter1> m_hardwareAdapter;
		ComPtr<ID3D12Device> m_device;
	};
}