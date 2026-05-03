#include "..\..\include\d3d12\D3D12Context.h"

void D3D12Context::Initialize(bool useWarpDevice)
{
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