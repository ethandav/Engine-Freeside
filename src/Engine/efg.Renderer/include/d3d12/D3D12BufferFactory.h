#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "D3D12BufferData.h"
#include "D3D12ResourceFactory.h"

using Microsoft::WRL::ComPtr;

namespace efg::d3d12
{
    class D3D12BufferFactory
    {
    public:
        void Initialize(D3D12ResourceFactory* resourceFactory);
        GpuBuffer CreateStaticBuffer(UINT64 sizeInBytes);
        GpuConstantBuffer CreateConstantBuffer(UINT64 sizeInBytes);
        GpuDepthBuffer CreateDepthBuffer(uint32_t width, uint32_t height);
        GpuStructuredBuffer CreateStructuredBufferUpload(uint32_t elementCount, uint32_t elementStride);
        GpuConstantBufferArena CreateConstantBufferArena(UINT64 capacityInBytes);
        GpuUploadBufferArena CreateUploadBufferArena(UINT64 capacityInBytes);

        void UpdateConstantBuffer(GpuConstantBuffer& buffer, const void* data, UINT64 sizeInBytes);
        D3D12_GPU_VIRTUAL_ADDRESS CopyToConstantBufferArena(GpuConstantBufferArena& arena, const void* data, UINT64 sizeInBytes);
        D3D12_GPU_VIRTUAL_ADDRESS CopyToUploadBufferArena(GpuUploadBufferArena& arena, const void* data, UINT64 sizeInBytes, UINT64 alignment = 16);
        GpuUploadBufferAllocation AllocateUploadBufferArena(GpuUploadBufferArena& arena, UINT64 sizeInBytes, UINT64 alignment);

        void DestroyConstantBuffer(GpuConstantBuffer& buffer);
        void DestroyConstantBufferArena(GpuConstantBufferArena& arena);
        void DestroyStructuredBuffer(GpuStructuredBuffer& buffer);
        void DestroyUploadBufferArena(GpuUploadBufferArena& arena);
        void DestroyDepthBuffer(GpuDepthBuffer& buffer);
        void DestroyGpuBuffer(GpuBuffer& buffer);
    private:
        D3D12ResourceFactory* m_resourceFactory = nullptr;
    };
}