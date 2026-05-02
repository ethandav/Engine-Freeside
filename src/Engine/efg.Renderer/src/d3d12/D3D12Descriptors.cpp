#include "..\..\include\renderer.h"

UINT CreateRTVDescriptorHeap(ID3D12Device* device, const UINT FrameCount, ID3D12DescriptorHeap** rtvHeap)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    D3D12_THROW_IF_FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeap)));

    return device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void Renderer::CreateDescriptorHeaps()
{
    m_rtvDescriptorSize = CreateRTVDescriptorHeap(m_device.Get(), FrameCount, m_rtvHeap.GetAddressOf());
}