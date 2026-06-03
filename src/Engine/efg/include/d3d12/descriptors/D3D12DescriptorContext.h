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
		void CreateRTVDescriptorHeap(const UINT Count = MaxRTVHeapSize);
		void CreateDSVDescriptorHeap(const UINT Count = MaxDSVHeapSize);
		void CreateShaderVisibleCBVSRVUAVDescriptorHeap(const UINT Count = MaxShaderVisibleHeapSize);
		void CreateCPUOnlyCBVSRVUAVDescriptorHeap(const UINT Count = MaxCpuOnlyCbvSrvUavHeapSize);
		void CreateSamplerDescriptorHeap(const UINT Count = MaxSamplerHeapSize);
		GpuDescriptorArena CreateFrameDescriptorArena(const uint32_t frame);
		CpuDescriptorAllocation AllocateRTV(const uint32_t count = 1);
		CpuDescriptorAllocation AllocateDSV(const uint32_t count = 1);
		GpuDescriptorAllocation AllocateShaderVisibleCBVSRVUAV(const uint32_t count = 1);
		CpuDescriptorAllocation AllocateCpuOnlyCBVSRVUAV(const uint32_t count = 1);
		GpuDescriptorAllocation AllocateSampler(const uint32_t count = 1);
		GpuDescriptorTable AllocateShaderVisibleTableFromFrameArena(GpuDescriptorArena& arena, uint32_t count);
		ID3D12DescriptorHeap* GetCBVSRVUAVHeap() const;
		ID3D12DescriptorHeap* GetSamplerHeap() const;
		UINT GetCBVSRVUAVDescriptorSize();

	private:
		static constexpr UINT MaxRTVHeapSize = 32;
		static constexpr UINT MaxCpuOnlyCbvSrvUavHeapSize = 1024;
		static constexpr UINT MaxShaderVisibleHeapSize = PersistentShaderVisibleDescriptorCapacity + NumFramesInFlight * ShaderVisibleDescriptorsPerFrame;
		static constexpr UINT MaxDSVHeapSize = 1024;
		static constexpr UINT MaxSamplerHeapSize = 16;

		class Heap
		{
		public:
			void Create(ID3D12Device* device, uint32_t count, D3D12_DESCRIPTOR_HEAP_DESC desc);
			ID3D12DescriptorHeap* Get() const;
			uint32_t GetUsed() const;
			uint32_t GetDescriptorSize() const;
			bool HasSpaceForCount(uint32_t count) const;
			bool IsShaderVisible() const;
			D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandleAtIndex(uint32_t index) const;
			D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandleAtIndex(uint32_t index) const;
			void Allocate(CpuDescriptorAllocation& allocation);
			void Allocate(GpuDescriptorAllocation& allocation);
		private:
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart = {};
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart = {};
			UINT m_descriptorSize = 0;
			UINT m_used = 0;
			UINT m_capacity = 0;
		};

		Heap m_rtvHeap;
		Heap m_shaderVisibleHeap;
		Heap m_cpuOnlyCbvSrvUavHeap;
		Heap m_dsvHeap;
		Heap m_samplerHeap;

		ID3D12Device* m_device = nullptr;
	};
}