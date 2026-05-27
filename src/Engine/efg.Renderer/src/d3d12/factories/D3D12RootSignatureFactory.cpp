#include "..\..\..\include\d3d12\factories\D3D12RootSignatureFactory.h"
#include "..\..\..\include\d3d12\core\D3D12Error.h"

namespace efg::d3d12
{
	void D3D12RootSignatureFactory::Initialize(ID3D12Device* device)
	{
		m_device = device;
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature> D3D12RootSignatureFactory::CreateRootSignature(CD3DX12_ROOT_SIGNATURE_DESC desc) const
	{
		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> error;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

        D3D12_THROW_IF_FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &error));
        D3D12_THROW_IF_FAILED(m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf())));
		return rootSignature;
	}
}