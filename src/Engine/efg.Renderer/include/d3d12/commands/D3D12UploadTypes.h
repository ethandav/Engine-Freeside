#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <vector>

namespace efg::d3d12
{
	struct PendingBufferUpload
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> destination;
		Microsoft::WRL::ComPtr<ID3D12Resource> upload;
		UINT64 sizeInBytes = 0;
		D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_COMMON;
	};

	struct PendingTextureUpload
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> destination;
		Microsoft::WRL::ComPtr<ID3D12Resource> upload;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	};

	struct UploadedResource
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> destination;
		D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_COMMON;
	};

	struct UploadTicket
	{
		UINT64 copyFenceValue = 0;
		std::vector<UploadedResource> resources = {};
	};

	struct UploadBatch
	{
		UINT64 copyFenceValue = 0;
		std::vector<UploadedResource> resources = {};
		std::vector<PendingBufferUpload> bufferUploads = {};
		std::vector<PendingTextureUpload> textureUploads = {};
	};
}