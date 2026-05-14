#pragma once
#include "..\resources\D3D12GpuBuffer.h"
#include "..\resources\D3D12GpuArena.h"

#include <cstdint>
#include <wrl.h>
#include <d3d12.h>

namespace efg::d3d12
{
    class D3D12ResourceFactory;

    class D3D12BufferFactory
    {
    public:
        void Initialize(D3D12ResourceFactory* resourceFactory);
        GpuBuffer CreateStaticBuffer(uint64_t sizeInBytes);
        GpuConstantBuffer CreateConstantBuffer(uint64_t sizeInBytes);
        GpuStructuredBuffer CreateStructuredBufferUpload(uint32_t elementCount, uint32_t elementStride);
        GpuConstantBufferArena CreateConstantBufferArena(uint64_t capacityInBytes);
        GpuUploadBufferArena CreateUploadBufferArena(uint64_t capacityInBytes);

        void UpdateConstantBuffer(GpuConstantBuffer& buffer, const void* data, uint64_t sizeInBytes);
        D3D12_GPU_VIRTUAL_ADDRESS CopyToConstantBufferArena(GpuConstantBufferArena& arena, const void* data, uint64_t sizeInBytes);
        D3D12_GPU_VIRTUAL_ADDRESS CopyToUploadBufferArena(GpuUploadBufferArena& arena, const void* data, uint64_t sizeInBytes, uint64_t alignment = 16);
        GpuUploadBufferAllocation AllocateUploadBufferArena(GpuUploadBufferArena& arena, uint64_t sizeInBytes, uint64_t alignment);

        void DestroyConstantBuffer(GpuConstantBuffer& buffer);
        void DestroyConstantBufferArena(GpuConstantBufferArena& arena);
        void DestroyStructuredBuffer(GpuStructuredBuffer& buffer);
        void DestroyUploadBufferArena(GpuUploadBufferArena& arena);
        void DestroyGpuBuffer(GpuBuffer& buffer);
    private:
        D3D12ResourceFactory* m_resourceFactory = nullptr;
    };
}