#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "D3D12BufferData.h"

using Microsoft::WRL::ComPtr;

class D3D12BufferFactory
{
public:
    GpuBuffer CreateStaticBuffer(ID3D12Device* device, const void* data, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState);
    GpuConstantBuffer CreateConstantBuffer(ID3D12Device* device, UINT64 sizeInBytes);
    GpuDepthBuffer CreateDepthBuffer(ID3D12Device* device, uint32_t width, uint32_t height);
    GpuConstantBufferArena CreateConstantBufferArena(ID3D12Device* device, UINT64 capacityInBytes);
    GpuStructuredBuffer CreateStructuredBufferUpload(ID3D12Device* device, uint32_t elementCount, uint32_t elementStride);
    void UpdateConstantBuffer(GpuConstantBuffer& buffer, const void* data, UINT64 sizeInBytes);
    D3D12_GPU_VIRTUAL_ADDRESS UploadConstantBufferArena(GpuConstantBufferArena& arena, const void* data, UINT64 sizeInBytes);
    void DestroyConstantBuffer(GpuConstantBuffer& buffer);
    void DestroyConstantBufferArena(GpuConstantBufferArena& arena);
    GpuUploadBufferArena CreateUploadBufferArena(ID3D12Device* device, UINT64 capacityInBytes);
    D3D12_GPU_VIRTUAL_ADDRESS UploadBufferArena(GpuUploadBufferArena& arena, const void* data, UINT64 sizeInBytes, UINT64 alignment = 16);
private:
};