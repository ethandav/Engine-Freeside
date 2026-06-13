#pragma once
#include "..\libraries\D3D12MaterialLibrary.h"
#include "..\libraries\D3D12MaterialTextureLibrary.h"
#include "..\factories\D3D12TextrureFactory.h"
#include "..\factories\D3D12ResourceFactory.h"
#include "..\factories\D3D12BufferFactory.h"
#include "..\commands\D3D12UploadContext.h"
#include "..\libraries\D3D12MeshLibrary.h"

#include "D3D12DeviceSystem.h"
#include "..\commands\D3D12CommandContext.h"

#include "..\..\..\..\Freeside.Assets\include\MaterialTypes.h"

namespace efg::d3d12
{
	class D3D12ResourceSystem
	{
	public:
		void Initialize(D3D12DeviceSystem* device);
		D3D12TextureFactory& TextureFactory();
		D3D12BufferFactory& BufferFactory();
		D3D12MaterialLibrary& Materials();
		D3D12MaterialTextureLibrary& MaterialTextures();
		D3D12MeshLibrary& Meshes();
		D3D12UploadContext& UploadContext();
		void CreateBuiltIns();
		Material CreateMaterial(const Freeside::MaterialDesc& materialDesc);
		Freeside::MaterialHandle RegisterMaterial(const Freeside::MaterialDesc& materialDesc);
		void UpdateMaterial(const Freeside::MaterialHandle& handle, const Freeside::MaterialDesc& desc);
		Freeside::MeshHandle CreateMesh(const Freeside::MeshDesc& mesh);
		Freeside::TextureHandle CreateMaterialTexture2d(const Freeside::TextureDesc& textureDesc);
		Freeside::TextureHandle CreateTextureCube(const std::array<Freeside::TextureDesc, 6> faces);
		void ProcessUploads(D3D12DirectCommandContext* commandContext);

#if defined(FREESIDE_EDITOR)
		uint64_t GetTextureID(Freeside::TextureHandle handle);
#endif

	private:
		D3D12ResourceFactory m_resourceFactory = {};
		D3D12BufferFactory m_bufferFactory = {};
		D3D12TextureFactory m_textureFactory = {};
		D3D12UploadContext m_uploadContext = {};
		D3D12MaterialLibrary m_materialLibrary = {};
		D3D12MeshLibrary m_meshLibrary;
		D3D12MaterialTextureLibrary m_textureLibrary = {};
	};
}