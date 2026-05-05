#pragma once

using Microsoft::WRL::ComPtr;

struct GpuBuffer
{
    UINT64 sizeInBytes = 0;
    ComPtr<ID3D12Resource> resource;
    ComPtr<ID3D12Resource> uploadResource;
};

struct GpuConstantBuffer
{
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    void* mappedData = nullptr;

    UINT64 sizeInBytes = 0;
    UINT64 alignedSizeInBytes = 0;
};

static UINT64 AlignConstantBufferSize(UINT64 size)
{
    return (size + 255) & ~255;
}