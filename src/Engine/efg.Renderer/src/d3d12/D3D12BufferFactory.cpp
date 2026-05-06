#include <d3dx12.h>
#include "..\..\include\d3d12\D3D12BufferFactory.h"
#include "..\..\include\d3d12\D3D12Error.h"
#include <assert.h>

GpuBuffer D3D12BufferFactory::CreateStaticBuffer(ID3D12Device* device, const void* data, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState)
{
    GpuBuffer buffer = {};
    buffer.sizeInBytes = sizeInBytes;
    auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);

    D3D12_THROW_IF_FAILED(device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(buffer.resource.GetAddressOf())
    ));

    auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    D3D12_THROW_IF_FAILED(device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(buffer.uploadResource.GetAddressOf())
    ));

    void* mappedData = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    D3D12_THROW_IF_FAILED(buffer.uploadResource->Map(
        0,
        &readRange,
        &mappedData
    ));

    memcpy(mappedData, data, static_cast<size_t>(sizeInBytes));
    buffer.uploadResource->Unmap(0, nullptr);

    return buffer;
}

GpuConstantBuffer D3D12BufferFactory::CreateConstantBuffer(ID3D12Device* device, UINT64 sizeInBytes)
{
    GpuConstantBuffer buffer = {};
    buffer.sizeInBytes = sizeInBytes;
    buffer.alignedSizeInBytes = AlignConstantBufferSize(sizeInBytes);
    auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(buffer.alignedSizeInBytes);

    D3D12_THROW_IF_FAILED(device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(buffer.resource.GetAddressOf())
    ));

    CD3DX12_RANGE readRange(0, 0);

    D3D12_THROW_IF_FAILED(buffer.resource->Map(
        0,
        &readRange,
        &buffer.mappedData
    ));

    return buffer;
}

void D3D12BufferFactory::UpdateConstantBuffer(GpuConstantBuffer& buffer, const void* data, UINT64 sizeInBytes)
{
    assert(sizeInBytes <= buffer.sizeInBytes);
    memcpy(buffer.mappedData, data, static_cast<size_t>(sizeInBytes));
}

D3D12_GPU_VIRTUAL_ADDRESS D3D12BufferFactory::UploadConstantBufferArena(GpuConstantBufferArena& arena, const void* data, UINT64 sizeInBytes)
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

    arena.resource.Reset();
    arena.capacityInBytes = 0;
    arena.currentOffset = 0;
}

GpuConstantBufferArena D3D12BufferFactory::CreateConstantBufferArena(ID3D12Device* device, UINT64 capacityInBytes)
{
    GpuConstantBufferArena arena = {};

    arena.capacityInBytes = AlignConstantBufferSize(capacityInBytes);
    arena.currentOffset = 0;
    auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(arena.capacityInBytes);

    D3D12_THROW_IF_FAILED(device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(arena.resource.GetAddressOf())
    ));

    CD3DX12_RANGE readRange(0, 0);
    void* mapped = nullptr;
    D3D12_THROW_IF_FAILED(arena.resource->Map(0, &readRange, &mapped));
    arena.mappedData = static_cast<uint8_t*>(mapped);

    return arena;
}