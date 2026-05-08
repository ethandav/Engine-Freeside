#pragma once
#include "..\..\include\d3d12\D3D12Context.h"

#include <d3d12.h>
#include <cstdint>

struct DescriptorAllocation
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};
	uint32_t index = 0;
};

class D3D12DescriptorContext
{
public:
	void Initialize(D3D12Context* graphicsContext);
	void CreateAllHeaps();
	UINT CreateRTVDescriptorHeap(const UINT Count = MaxRTVHeapSize);
	UINT CreateShaderResourceDescriptorHeap(const UINT Count = MaxShaderVisibleHeapSize);
	UINT CreateDSVDescriptorHeap(const UINT Count = MaxDSVHeapSize);
	UINT CreateSamplerDescriptorHeap(const UINT Count = MaxSamplerHeapSize);
	DescriptorAllocation AllocateRTV();
	DescriptorAllocation AllocateShaderVisible();
	DescriptorAllocation CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);
	DescriptorAllocation CreateShaderVisibleView(ID3D12Resource* resource, uint32_t elementCount, uint32_t elementStride);
	ID3D12DescriptorHeap* GetShaderVisibleHeap();

private:
	static constexpr UINT MaxRTVHeapSize = 32;
	static constexpr UINT MaxShaderVisibleHeapSize = 16;
	static constexpr UINT MaxDSVHeapSize = 1024;
	static constexpr UINT MaxSamplerHeapSize = 16;

	// RTV Heap
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHeapStart = {};
	UINT m_rtvDescriptorSize = 0;
	UINT m_rtvUsed = 0;
	UINT m_rtvCapacity = 0;

	// Shader Visible Heap
	ComPtr<ID3D12DescriptorHeap> m_shaderVisibleHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_shaderVisibleHeapCpuStart = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_shaderVisibleHeapGpuStart = {};
	UINT m_shaderVisibleDescriptorSize = 0;
	UINT m_shaderVisibleUsed = 0;
	UINT m_shaderVisibleCapacity = 0;

	// DSV Heap
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHeapStart = {};
	UINT m_dsvDescriptorSize = 0;
	UINT m_dsvUsed = 0;
	UINT m_dsvCapacity = 0;

	// Sampler Heap
	ComPtr<ID3D12DescriptorHeap> m_samplerHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_samplerHeapCpuStart = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_samplerHeapGpuStart = {};
	UINT m_samplerDescriptorSize = 0;
	UINT m_samplerUsed = 0;
	UINT m_samplerCapacity = 0;

	D3D12Context* m_graphicsContext = nullptr;
};