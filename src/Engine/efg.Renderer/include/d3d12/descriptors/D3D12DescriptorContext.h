#pragma once
#include "D3D12DescriptorAllocation.h"

#include <wrl.h>

namespace efg::d3d12
{
	class D3D12DescriptorContext
	{
	public:
		void Initialize(ID3D12Device* device);
		void CreateAllHeaps();
		UINT CreateRTVDescriptorHeap(const UINT Count = MaxRTVHeapSize);
		UINT CreateDSVDescriptorHeap(const UINT Count = MaxDSVHeapSize);
		UINT CreateCBVSRVUAVDescriptorHeap(const UINT Count = MaxShaderVisibleHeapSize);
		UINT CreateSamplerDescriptorHeap(const UINT Count = MaxSamplerHeapSize);
		DescriptorAllocation AllocateRTV();
		DescriptorAllocation AllocateDSV();
		DescriptorAllocation AllocateCBVSRVUAV();
		DescriptorAllocation AllocateSampler();
		DescriptorAllocation CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);
		DescriptorAllocation CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc);
		DescriptorAllocation CreateCBV(ID3D12Resource* resource, uint32_t sizeInBytes);
		DescriptorAllocation CreateUAV(ID3D12Resource* resource, uint32_t elementCount, uint32_t elementStride, ID3D12Resource* counterResource);
		DescriptorAllocation CreateStructuredBufferSRV(ID3D12Resource* resource, uint32_t elementCount, uint32_t elementStride);
		DescriptorAllocation CreateTexture2DSRV(ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels);
		DescriptorAllocation CreateSampler(const D3D12_SAMPLER_DESC& samplerDesc);
		ID3D12DescriptorHeap* GetCBVSRVUAVHeap() const;
		ID3D12DescriptorHeap* GetSamplerHeap() const;

	private:
		static constexpr UINT MaxRTVHeapSize = 32;
		static constexpr UINT MaxShaderVisibleHeapSize = 16;
		static constexpr UINT MaxDSVHeapSize = 1024;
		static constexpr UINT MaxSamplerHeapSize = 16;

		// RTV Heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHeapStart = {};
		UINT m_rtvDescriptorSize = 0;
		UINT m_rtvUsed = 0;
		UINT m_rtvCapacity = 0;

		// Shader Visible Heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_shaderVisibleHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_shaderVisibleHeapCpuStart = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_shaderVisibleHeapGpuStart = {};
		UINT m_shaderVisibleDescriptorSize = 0;
		UINT m_shaderVisibleUsed = 0;
		UINT m_shaderVisibleCapacity = 0;

		// DSV Heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHeapStart = {};
		UINT m_dsvDescriptorSize = 0;
		UINT m_dsvUsed = 0;
		UINT m_dsvCapacity = 0;

		// Sampler Heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_samplerHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_samplerHeapCpuStart = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_samplerHeapGpuStart = {};
		UINT m_samplerDescriptorSize = 0;
		UINT m_samplerUsed = 0;
		UINT m_samplerCapacity = 0;

		ID3D12Device* m_device = nullptr;
	};
}