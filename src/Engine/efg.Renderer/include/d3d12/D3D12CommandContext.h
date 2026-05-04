#pragma once
#include "D3D12Context.h"

class D3D12CommandContext
{
public:
	void Initialize(D3D12Context* graphicsContext);
	void BeginRecording(ID3D12CommandAllocator* commandAllocator);
	void EndRecording();
	ID3D12GraphicsCommandList* GetCommandList();
	ID3D12CommandQueue* GetCommandQueue();
	ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
private:
	void CreateCommandObjects();
	void CreateCommandList();
	void CreateCommandQueue();


	D3D12Context* m_graphicsContext = {};
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
};