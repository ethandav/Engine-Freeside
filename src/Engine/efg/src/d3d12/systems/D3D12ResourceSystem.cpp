#include "..\..\..\include\d3d12\systems\D3D12ResourceSystem.h"
#include "..\..\..\include\d3d12\core\D3D12Format.h"

namespace efg::d3d12
{
	void D3D12ResourceSystem::Initialize(D3D12DeviceSystem* device)
	{
		m_resourceFactory.Initialize(device->GraphicsContext().GetDevice());
		m_uploadContext.Initialize(&device->GraphicsContext(), &m_resourceFactory);
		m_bufferFactory.Initialize(&m_resourceFactory);
		m_textureFactory.Initialize(device->GraphicsContext().GetDevice(), &m_resourceFactory, &device->DescriptorFactory());
        CreateBuiltIns();
	}

	D3D12TextureFactory& D3D12ResourceSystem::TextureFactory()
	{
		return m_textureFactory;
	}

	D3D12BufferFactory& D3D12ResourceSystem::BufferFactory()
	{
		return m_bufferFactory;
	}

	D3D12MaterialLibrary& D3D12ResourceSystem::Materials()
	{
		return m_materialLibrary;
	}

	D3D12MaterialTextureLibrary& D3D12ResourceSystem::MaterialTextures()
	{
		return m_textureLibrary;
	}

	D3D12MeshLibrary& D3D12ResourceSystem::Meshes()
	{
		return m_meshLibrary;
	}

    D3D12UploadContext& D3D12ResourceSystem::UploadContext()
    {
        return m_uploadContext;
    }

    void D3D12ResourceSystem::CreateBuiltIns()
    {
        {
            DecodedImage image = m_imageLoader.CreateSolidColorImage(100, 100, 100, 255);
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, ToDxgiFormat(image.format), DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_textureLibrary.RegisterDefaultMaterialTexture2D(texture);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        {
            DecodedImage image = m_imageLoader.CreateSolidColorImage(128, 128, 255, 255);
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, DXGI_FORMAT_R8G8B8A8_UNORM, DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_textureLibrary.RegisterDefaultNormalTexture2D(texture);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        {
            DecodedImage image = m_imageLoader.CreateSolidColorImage(128, 128, 128, 255);
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, DXGI_FORMAT_R8_UNORM, DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_textureLibrary.RegisterDefaultHeightTexture2D(texture);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        {
            std::array<DecodedImage, 6> faces =
            {
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/right.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/left.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/top.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/bottom.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/front.jpg"),
                m_imageLoader.LoadImageWithWIC(L"assets/textures/skybox/back.jpg"),
            };
            /*
            std::array<DecodedImage, 6> faces =
            {
                m_imageLoader.CreateSolidColorImage(255, 0, 0, 255),
                m_imageLoader.CreateSolidColorImage(255, 0, 0, 255),
                m_imageLoader.CreateSolidColorImage(0, 255, 0, 255),
                m_imageLoader.CreateSolidColorImage(0, 255, 0, 255),
                m_imageLoader.CreateSolidColorImage(0, 0, 255, 255),
                m_imageLoader.CreateSolidColorImage(0, 0, 255, 255),
            };
            */
            GpuTextureCube texture = m_textureFactory.CreateTextureCube(faces[0].width, faces[0].height, DescriptorVisibility::CpuOnlyAndShaderVisible, DXGI_FORMAT_R8G8B8A8_UNORM);
            m_textureLibrary.RegisterDefaultSkyboxTexture(texture);
            m_uploadContext.QueueTextureCubeUpload(texture.resource.Get(), faces, texture.resource.Get()->GetDesc(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }
    }

    Freeside::MaterialHandle D3D12ResourceSystem::RegisterMaterial(const Freeside::MaterialDesc& matDesc)
    {
        Material material = {};
        if (!matDesc.baseColorTexturePath.empty())
        {
            DecodedImage image = m_imageLoader.LoadImageWithWIC(matDesc.baseColorTexturePath.c_str());
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, ToDxgiFormat(image.format), DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            material.baseColorTexture = m_textureLibrary.RegisterMaterialTexture2D(texture);
        }

        if (!matDesc.normalTexturePath.empty())
        {
            DecodedImage image = m_imageLoader.LoadImageWithWIC(matDesc.normalTexturePath.c_str());
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, DXGI_FORMAT_R8G8B8A8_UNORM, DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            material.normalTexture = m_textureLibrary.RegisterMaterialTexture2D(texture);
        }

        if (!matDesc.heightTexturePath.empty())
        {
            DecodedImage image = m_imageLoader.LoadHeightMapWithWIC(matDesc.heightTexturePath.c_str());
            GpuTexture2D texture = m_textureFactory.CreateTexture2D(image.width, image.height, DXGI_FORMAT_R8_UNORM, DescriptorVisibility::CpuOnlyAndShaderVisible);
            m_uploadContext.QueueTexture2DUpload(texture.resource.Get(), image.pixels.data(), texture.resource.Get()->GetDesc(), image.rowPitch, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            material.heightTexture = m_textureLibrary.RegisterMaterialTexture2D(texture);
        }

        MaterialConstants materialConstants{
            Freeside::Math::Vec4(matDesc.baseColor.x, matDesc.baseColor.y, matDesc.baseColor.z, 1.0f),
            Freeside::Math::Vec4(matDesc.specular.x, matDesc.specular.y, 0.0f, 0.0f),
            Freeside::Math::Vec4(matDesc.uvScale.x, matDesc.uvScale.y, 0.0f, 0.0f)
        };
        material.constants = materialConstants;

        Freeside::MaterialHandle handle = m_materialLibrary.RegisterMaterial(material);
        return handle;
    }

    Freeside::MeshHandle D3D12ResourceSystem::CreateMesh(const Freeside::MeshDesc& mesh)
    {
        Freeside::MeshHandle handle = m_meshLibrary.RegisterMesh(mesh);
        GpuBuffer vertexBuffer = m_bufferFactory.CreateStaticBuffer((mesh.vertices.size() * sizeof(Freeside::Vertex)));
        GpuBuffer indexBuffer = m_bufferFactory.CreateStaticBuffer((mesh.indices.size() * sizeof(uint32_t)));
        m_meshLibrary.SetVertexBuffer(handle, vertexBuffer);
        m_meshLibrary.SetIndexBuffer(handle, indexBuffer);
        m_uploadContext.QueueBufferUpload(vertexBuffer.resource.Get(), mesh.vertices.data(), vertexBuffer.sizeInBytes, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        m_uploadContext.QueueBufferUpload(indexBuffer.resource.Get(), mesh.indices.data(), indexBuffer.sizeInBytes, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        return handle;
    }

    void D3D12ResourceSystem::ProcessUploads(D3D12DirectCommandContext* commandContext)
    {
        if (m_uploadContext.queueSize > 0)
        {
            UploadTicket uploadTicket = m_uploadContext.FlushUploads();
            m_uploadContext.copyfence.WaitForQueue(commandContext->GetDirectCommandQueue(), uploadTicket.copyFenceValue);
            for (const auto& upload : uploadTicket.resources)
            {
                commandContext->ResourceBarrierTransition(upload.destination.Get(), D3D12_RESOURCE_STATE_COPY_DEST, upload.finalState);
            }
        }
        if (m_uploadContext.pendingBatchesSize > 0)
        {
            m_uploadContext.RetireCompletedUploads();
        }
    }
}