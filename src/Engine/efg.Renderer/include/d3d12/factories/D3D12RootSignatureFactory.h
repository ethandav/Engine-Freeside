#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

namespace efg::d3d12
{
	class D3D12RootSignatureFactory
	{
	public:
		void Initialize(ID3D12Device* device);
		Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(CD3DX12_ROOT_SIGNATURE_DESC desc) const;
	private:
		ID3D12Device* m_device = nullptr;
	};
}