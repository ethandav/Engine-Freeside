#pragma once
#include "..\..\include\d3d12\D3D12Context.h"

#include <d3d12.h>

class D3D12DescriptorContext
{
public:
	void Initialize(D3D12Context* graphicsContext);
	void CreateAllHeaps();
	UINT CreateRTVDescriptorHeap(const UINT Count = MaxRTVHeapSize);
	UINT CreateShaderResourceDescriptorHeap(const UINT Count = MaxShaderVisibleHeapSize);
	UINT CreateDSVDescriptorHeap(const UINT Count = MaxDSVHeapSize);
	UINT CreateSamplerDescriptorHeap(const UINT Count = MaxSamplerHeapSize);
	D3D12_CPU_DESCRIPTOR_HANDLE AllocateRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);

private:
	static constexpr UINT MaxRTVHeapSize = 32;
	static constexpr UINT MaxShaderVisibleHeapSize = 16;
	static constexpr UINT MaxDSVHeapSize = 1024;
	static constexpr UINT MaxSamplerHeapSize = 16;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHeapStart = {};
	UINT m_rtvDescriptorSize = 0;
	UINT m_rtvUsed = 0;
	UINT m_rtvCapacity = 0;
	D3D12Context* m_graphicsContext = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
};