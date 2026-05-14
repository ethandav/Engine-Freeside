#include "..\..\..\include\d3d12\factories\D3D12BufferFactory.h"
#include "..\..\..\include\d3d12\factories\D3D12ResourceFactory.h"
#include "..\..\..\include\d3d12\resources\D3D12GpuAlignment.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"

#include <stdexcept>
#include <assert.h>
#include <d3dx12.h>

namespace efg::d3d12
{
    void D3D12BufferFactory::Initialize(D3D12ResourceFactory* resourceFactory)
    {
        m_resourceFactory = resourceFactory;
    }

    GpuBuffer D3D12BufferFactory::CreateStaticBuffer(UINT64 sizeInBytes)
    {
        GpuBuffer buffer = {};
        buffer.sizeInBytes = sizeInBytes;
        buffer.resource = m_resourceFactory->CreateCommittedDefaultBufferResource(sizeInBytes, D3D12_RESOURCE_STATE_COMMON);

        return buffer;
    }

    GpuConstantBuffer D3D12BufferFactory::CreateConstantBuffer(UINT64 sizeInBytes)
    {
        GpuConstantBuffer buffer = {};
        buffer.sizeInBytes = sizeInBytes;
        buffer.alignedSizeInBytes = AlignConstantBufferSize(sizeInBytes);
        buffer.resource = m_resourceFactory->CreateCommittedUploadBufferResource(sizeInBytes);
        CD3DX12_RANGE readRange(0, 0);
        D3D12_THROW_IF_FAILED(buffer.resource->Map(0, &readRange, &buffer.mappedData));

        return buffer;
    }

    void D3D12BufferFactory::UpdateConstantBuffer(GpuConstantBuffer& buffer, const void* data, UINT64 sizeInBytes)
    {
        assert(sizeInBytes <= buffer.sizeInBytes);
        memcpy(buffer.mappedData, data, static_cast<size_t>(sizeInBytes));
    }

    D3D12_GPU_VIRTUAL_ADDRESS D3D12BufferFactory::CopyToConstantBufferArena(GpuConstantBufferArena& arena, const void* data, UINT64 sizeInBytes)
    {
        const UINT64 alignedSize = AlignConstantBufferSize(sizeInBytes);

        if (arena.currentOffset + alignedSize > arena.capacityInBytes)
        {
            throw std::runtime_error("Constant buffer arena out of memory.");
        }

        const UINT64 offset = arena.currentOffset;
        std::memcpy(arena.mappedData + offset, data, static_cast<size_t>(sizeInBytes));
        arena.currentOffset += alignedSize;

        return arena.GetGpuAddress(offset);
    }

    GpuConstantBufferArena D3D12BufferFactory::CreateConstantBufferArena(UINT64 capacityInBytes)
    {
        GpuConstantBufferArena arena = {};

        arena.capacityInBytes = AlignConstantBufferSize(capacityInBytes);
        arena.currentOffset = 0;
        arena.resource = m_resourceFactory->CreateCommittedUploadBufferResource(arena.capacityInBytes);

        CD3DX12_RANGE readRange(0, 0);
        void* mapped = nullptr;
        D3D12_THROW_IF_FAILED(arena.resource->Map(0, &readRange, &mapped));
        arena.mappedData = static_cast<uint8_t*>(mapped);

        return arena;
    }

    GpuStructuredBuffer D3D12BufferFactory::CreateStructuredBufferUpload(uint32_t elementCount, uint32_t elementStride)
    {
        GpuStructuredBuffer buffer = {};

        buffer.elementCount = elementCount;
        buffer.elementStride = elementStride;
        buffer.sizeInBytes = static_cast<uint64_t>(elementCount) * elementStride;
        buffer.resource = m_resourceFactory->CreateCommittedUploadBufferResource(buffer.sizeInBytes);

        CD3DX12_RANGE readRange(0, 0);
        D3D12_THROW_IF_FAILED(buffer.resource->Map(0, &readRange, reinterpret_cast<void**>(&buffer.mappedData)));

        return buffer;
    }

    GpuUploadBufferArena D3D12BufferFactory::CreateUploadBufferArena(UINT64 capacityInBytes)
    {
        GpuUploadBufferArena arena = {};
        arena.capacityInBytes = AlignUp(capacityInBytes, 256);
        arena.currentOffset = 0;
        arena.resource = m_resourceFactory->CreateCommittedUploadBufferResource(arena.capacityInBytes);

        CD3DX12_RANGE readRange(0, 0);
        void* mapped = nullptr;
        D3D12_THROW_IF_FAILED(arena.resource->Map(0, &readRange, &mapped));
        arena.mappedData = static_cast<uint8_t*>(mapped);

        return arena;
    }

    D3D12_GPU_VIRTUAL_ADDRESS D3D12BufferFactory::CopyToUploadBufferArena(GpuUploadBufferArena& arena, const void* data, UINT64 sizeInBytes, UINT64 alignment)
    {
        GpuUploadBufferAllocation allocation = AllocateUploadBufferArena(arena, sizeInBytes, alignment);
        std::memcpy(allocation.cpu, data, static_cast<size_t>(sizeInBytes));

        return allocation.gpu;
    }

    GpuUploadBufferAllocation D3D12BufferFactory::AllocateUploadBufferArena(GpuUploadBufferArena& arena, UINT64 sizeInBytes, UINT64 alignment)
    {
        const UINT64 alignedOffset = AlignUp(arena.currentOffset, alignment);

        if (alignedOffset + sizeInBytes > arena.capacityInBytes)
        {
            throw std::runtime_error("Upload buffer arena out of memory.");
        }

        GpuUploadBufferAllocation allocation = {};
        allocation.cpu = arena.mappedData + alignedOffset;
        allocation.gpu = arena.GetGpuAddress(alignedOffset);
        allocation.sizeInBytes = sizeInBytes;
        allocation.offset = alignedOffset;

        arena.currentOffset = alignedOffset + sizeInBytes;

        return allocation;
    }

    void D3D12BufferFactory::DestroyConstantBuffer(GpuConstantBuffer& buffer)
    {
        if (buffer.resource && buffer.mappedData)
        {
            buffer.resource->Unmap(0, nullptr);
            buffer.mappedData = nullptr;
        }

        buffer.resource.Reset();
        buffer.sizeInBytes = 0;
        buffer.alignedSizeInBytes = 0;
    }

    void D3D12BufferFactory::DestroyConstantBufferArena(GpuConstantBufferArena& arena)
    {
        if (arena.resource && arena.mappedData)
        {
            arena.resource->Unmap(0, nullptr);
            arena.mappedData = nullptr;
        }

        arena.Reset();
        arena.resource.Reset();
        arena.capacityInBytes = 0;
        arena.currentOffset = 0;
    }

    void D3D12BufferFactory::DestroyStructuredBuffer(GpuStructuredBuffer& buffer)
    {
        if (buffer.resource && buffer.mappedData)
        {
            buffer.resource->Unmap(0, nullptr);
            buffer.mappedData = nullptr;
        }

        buffer.resource.Reset();
        buffer.cpuSrv.ptr = 0;
        buffer.gpuSrv.ptr = 0;
        buffer.elementCount = 0;
        buffer.elementStride = 0;
        buffer.sizeInBytes = 0;
    }

    void D3D12BufferFactory::DestroyUploadBufferArena(GpuUploadBufferArena& arena)
    {
        if (arena.resource && arena.mappedData)
        {
            arena.resource->Unmap(0, nullptr);
            arena.mappedData = nullptr;
        }

        arena.Reset();
        arena.resource.Reset();
        arena.capacityInBytes = 0;
        arena.currentOffset = 0;
    }

    void D3D12BufferFactory::DestroyGpuBuffer(GpuBuffer& buffer)
    {
        if (buffer.resource)
        {
            buffer.resource->Unmap(0, nullptr);
        }

        buffer.resource.Reset();
    }
}