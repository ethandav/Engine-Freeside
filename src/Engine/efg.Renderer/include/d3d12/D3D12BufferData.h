#pragma once

using Microsoft::WRL::ComPtr;

struct GpuBuffer
{
    UINT64 sizeInBytes = 0;
    ComPtr<ID3D12Resource> resource;
    ComPtr<ID3D12Resource> uploadResource;
};