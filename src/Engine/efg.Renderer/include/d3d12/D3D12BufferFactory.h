#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "D3D12BufferData.h"

using Microsoft::WRL::ComPtr;

class D3D12BufferFactory
{
public:
    GpuBuffer CreateStaticBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const void* data, UINT64 sizeInBytes, D3D12_RESOURCE_STATES finalState);
private:
};