#pragma once
#include "..\libraries\D3D12MaterialLibrary.h"
#include "..\libraries\D3D12MaterialTextureLibrary.h"
#include "..\factories\D3D12TextrureFactory.h"
#include "..\factories\D3D12ResourceFactory.h"
#include "..\factories\D3D12BufferFactory.h"
#include "..\commands\D3D12UploadContext.h"
#include "..\libraries\D3D12MeshLibrary.h"

#include "..\..\render\ImageLoader.h"

#include "D3D12DeviceSystem.h"
#include "..\commands\D3D12CommandContext.h"

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
		Freeside::MaterialHandle RegisterMaterial(const Freeside::MaterialDesc& materialDesc);
		Freeside::MeshHandle CreateMesh(const Freeside::MeshData& mesh);
		void ProcessUploads(D3D12DirectCommandContext* commandContext);

	private:
		D3D12ResourceFactory m_resourceFactory = {};
		D3D12BufferFactory m_bufferFactory = {};
		D3D12TextureFactory m_textureFactory = {};
		D3D12UploadContext m_uploadContext = {};
		D3D12MaterialLibrary m_materialLibrary = {};
		D3D12MeshLibrary m_meshLibrary;
		D3D12MaterialTextureLibrary m_textureLibrary = {};
		ImageLoader m_imageLoader = {};
	};
}