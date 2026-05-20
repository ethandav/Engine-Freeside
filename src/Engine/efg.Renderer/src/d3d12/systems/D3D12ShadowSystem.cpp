#include "..\..\..\include\d3d12\systems\D3D12ShadowSystem.h"
#include "..\..\..\include\d3d12\factories\D3D12TextrureFactory.h"

namespace efg::d3d12
{
	void D3D12ShadowSystem::Initialize(D3D12TextureFactory* textureFactory)
	{
		m_textureFactory = textureFactory;
	}

	void D3D12ShadowSystem::Update(const FramePacket& scene)
	{

	}
}