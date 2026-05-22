#pragma once
#include "D3D12DescriptorAllocation.h"
#include "D3D12GpuDescriptorTable.h"
#include "..\resources\D3D12GpuArena.h"
#include "..\frame\D3D12FrameConfig.h"

#include <wrl.h>

namespace efg::d3d12
{
	static constexpr uint32_t PersistentShaderVisibleDescriptorCapacity = 1024;

	class D3D12DescriptorContext
	{
	public:
		void Initialize(ID3D12Device* device);
		void CreateAllHeaps();
		UINT CreateRTVDescriptorHeap(const UINT Count = MaxRTVHeapSize);
		UINT CreateDSVDescriptorHeap(const UINT Count = MaxDSVHeapSize);
		UINT CreateShaderVisibleCBVSRVUAVDescriptorHeap(const UINT Count = MaxShaderVisibleHeapSize);
		UINT CreateCPUOnlyCBVSRVUAVDescriptorHeap(const UINT Count = MaxShaderVisibleHeapSize);
		UINT CreateSamplerDescriptorHeap(const UINT Count = MaxSamplerHeapSize);
		GpuDescriptorArena CreateDescriptorArena(const uint32_t frame);
		DescriptorAllocation AllocateRTV();
		DescriptorAllocation AllocateDSV();
		DescriptorAllocation AllocateShaderVisibleCBVSRVUAV();
		DescriptorAllocation AllocateCpuOnlyCBVSRVUAV();
		DescriptorAllocation AllocateSampler();
		GpuDescriptorTable AllocateCBVSRVUAVTable(uint32_t count);
		GpuDescriptorTable AllocateShaderVisibleTableFromFrameArena(GpuDescriptorArena& arena, uint32_t count);
		ID3D12DescriptorHeap* GetCBVSRVUAVHeap() const;
		ID3D12DescriptorHeap* GetSamplerHeap() const;
		UINT GetCBVSRVUAVDescriptorSize();

	private:
		static constexpr UINT MaxRTVHeapSize = 32;
		static constexpr UINT MaxShaderVisibleHeapSize = PersistentShaderVisibleDescriptorCapacity + NumFramesInFlight * ShaderVisibleDescriptorsPerFrame;
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

		// CPU Only CBV SRV UAV Heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cpuOnlyCbvSrvUavHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuOnlyCbvSrvUavHeapCpuStart = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_cpuOnlyCbvSrvUavHeapGpuStart = {};
		UINT m_cpuOnlyCbvSrvUavDescriptorSize = 0;
		UINT m_cpuOnlyCbvSrvUavUsed = 0;
		UINT m_cpuOnlyCbvSrvUavCapacity = 0;

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