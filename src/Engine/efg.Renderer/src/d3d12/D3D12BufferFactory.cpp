#include <d3dx12.h>
#include "..\..\include\d3d12\D3D12BufferFactory.h"
#include "..\..\include\d3d12\D3D12Error.h"

GpuBuffer D3D12BufferFactory::CreateStaticBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const void* data, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState)
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