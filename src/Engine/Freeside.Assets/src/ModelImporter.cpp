#include "..\include\ModelImporter.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define _CRT_SECURE_NO_WARNINGS

#include "..\ThirdParty\tinyGLTF\tiny_gltf.h"

#include <stdio.h>
#include <stdexcept>
#include "..\..\Freeside.Core\include\math\Meshes.h"
#include "..\include\MaterialTypes.h"
#include "..\include\Format.h"

namespace Freeside::Assets
{
    static bool HasAttribute(const tinygltf::Primitive& primitive, const std::string& name)
    {
        return primitive.attributes.find(name) != primitive.attributes.end();
    }

    static const tinygltf::Accessor& GetAttributeAccessor(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const std::string& name)
    {
        auto it = primitive.attributes.find(name);
        if (it == primitive.attributes.end())
            throw std::runtime_error("Missing glTF attribute: " + name);

        return model.accessors[it->second];
    }

    static const uint8_t* GetAccessorData(const tinygltf::Model& model, const tinygltf::Accessor& accessor)
    {
        const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        return buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
    }

    static size_t GetAccessorStride(const tinygltf::Model& model, const tinygltf::Accessor& accessor)
    {
        const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];

        int stride = accessor.ByteStride(bufferView);
        if (stride <= 0)
            throw std::runtime_error("Invalid glTF accessor stride.");

        return static_cast<size_t>(stride);
    }

    static void ReadVertices(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::vector<Freeside::Vertex>& outVertices)
    {
        const tinygltf::Accessor& positionAccessor = GetAttributeAccessor(model, primitive, "POSITION");

        if (positionAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || positionAccessor.type != TINYGLTF_TYPE_VEC3)
        {
            throw std::runtime_error("POSITION must be float3.");
        }

        const size_t vertexCount = positionAccessor.count;
        outVertices.resize(vertexCount);

        const uint8_t* positionData = GetAccessorData(model, positionAccessor);
        const size_t positionStride = GetAccessorStride(model, positionAccessor);

        for (size_t i = 0; i < vertexCount; ++i)
        {
            const float* p = reinterpret_cast<const float*>(positionData + i * positionStride);

            outVertices[i].position = Freeside::Math::Vec3(
                p[0],
                p[1],
                p[2]
            );
        }

        if (HasAttribute(primitive, "NORMAL"))
        {
            const tinygltf::Accessor& normalAccessor = GetAttributeAccessor(model, primitive, "NORMAL");

            const uint8_t* normalData = GetAccessorData(model, normalAccessor);
            const size_t normalStride = GetAccessorStride(model, normalAccessor);

            for (size_t i = 0; i < vertexCount; ++i)
            {
                const float* n = reinterpret_cast<const float*>(normalData + i * normalStride);

                outVertices[i].normal = Freeside::Math::Vec3(
                    n[0],
                    n[1],
                    n[2]
                );
            }
        }

        if (HasAttribute(primitive, "TEXCOORD_0"))
        {
            const tinygltf::Accessor& uvAccessor = GetAttributeAccessor(model, primitive, "TEXCOORD_0");

            const uint8_t* uvData = GetAccessorData(model, uvAccessor);
            const size_t uvStride = GetAccessorStride(model, uvAccessor);

            for (size_t i = 0; i < vertexCount; ++i)
            {
                const float* uv = reinterpret_cast<const float*>(uvData + i * uvStride);

                outVertices[i].uv = Freeside::Math::Vec2(
                    uv[0],
                    uv[1]
                );
            }
        }

        if (HasAttribute(primitive, "TANGENT"))
        {
            const tinygltf::Accessor& tangentAccessor = GetAttributeAccessor(model, primitive, "TANGENT");

            const uint8_t* tangentData = GetAccessorData(model, tangentAccessor);
            const size_t tangentStride = GetAccessorStride(model, tangentAccessor);

            for (size_t i = 0; i < vertexCount; ++i)
            {
                const float* t = reinterpret_cast<const float*>(tangentData + i * tangentStride);

                outVertices[i].tangent = Freeside::Math::Vec3(
                    t[0],
                    t[1],
                    t[2]
                );

                // t[3] is handedness. You may want this later for normal mapping.
            }
        }
    }

    static void ReadIndices(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::vector<uint32_t>& outIndices)
    {
        if (primitive.indices < 0)
        {
            // Non-indexed mesh. Generate sequential indices.
            const tinygltf::Accessor& positionAccessor = GetAttributeAccessor(model, primitive, "POSITION");

            outIndices.resize(positionAccessor.count);

            for (uint32_t i = 0; i < static_cast<uint32_t>(positionAccessor.count); ++i)
                outIndices[i] = i;

            return;
        }

        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

        const uint8_t* indexData = GetAccessorData(model, indexAccessor);
        const size_t indexStride = GetAccessorStride(model, indexAccessor);

        outIndices.resize(indexAccessor.count);

        for (size_t i = 0; i < indexAccessor.count; ++i)
        {
            const uint8_t* src = indexData + i * indexStride;

            switch (indexAccessor.componentType)
            {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                outIndices[i] = *reinterpret_cast<const uint8_t*>(src);
                break;

            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                outIndices[i] = *reinterpret_cast<const uint16_t*>(src);
                break;

            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                outIndices[i] = *reinterpret_cast<const uint32_t*>(src);
                break;

            default:
                throw std::runtime_error("Unsupported glTF index component type.");
            }
        }
    }

    static void ImportMeshes(const tinygltf::Model& model, ImportedModel& outModel)
    {
        for (const tinygltf::Mesh& mesh : model.meshes)
        {
            ImportedMesh outMesh = {};
            for (const tinygltf::Primitive& primitive : mesh.primitives)
            {
                if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
                    continue;

                ImportedPrimitive outPrimitive = {};
                outPrimitive.materialIndex = primitive.material;

                ReadVertices(model, primitive, outPrimitive.meshData.vertices);
                ReadIndices(model, primitive, outPrimitive.meshData.indices);

                bool hasTangents = primitive.attributes.find("TANGENT") != primitive.attributes.end();

                if (!hasTangents)
                {
                    Freeside::Math::GenerateTangents(outPrimitive.meshData.vertices, outPrimitive.meshData.indices);
                }
                outMesh.primitives.push_back(outPrimitive);
            }
            outModel.meshes.push_back(std::move(outMesh));
        }
    }

    static void ImportTextures(const tinygltf::Model& model, ImportedModel& outModel)
    {
        outModel.textures.reserve(model.images.size());

        for (const tinygltf::Image& image : model.images)
        {
            Freeside::TextureDesc desc = {};
            desc.width = image.width;
            desc.height = image.height;
            desc.format = Format::R8G8B8A8_UNorm;

            const int srcChannels =  image.component;
            const int dstChannels = 4;

            desc.pixels.resize(desc.width * desc.height * dstChannels);

            for (uint32_t i = 0; i < desc.width* desc.height; ++i)
            {
                const uint8_t* src = &image.image[i * srcChannels];
                uint8_t* dst = &desc.pixels[i * dstChannels];

                dst[0] = srcChannels > 0 ? src[0] : 255;
                dst[1] = srcChannels > 1 ? src[1] : dst[0];
                dst[2] = srcChannels > 2 ? src[2] : dst[0];
                dst[3] = srcChannels > 3 ? src[3] : 255;
            }

            desc.rowPitch = desc.width * dstChannels;

            outModel.textures.push_back(std::move(desc));
        }
    }

    static int GetImageIndexFromTextureIndex(const tinygltf::Model& model, int textureIndex)
    {
        if (textureIndex < 0)
            return -1;

        const tinygltf::Texture& texture = model.textures[textureIndex];
        return texture.source;
    }

    static void ImportMaterials(const tinygltf::Model& model, ImportedModel& outModel)
    {
        outModel.materials.reserve(model.materials.size());

        for (const tinygltf::Material& material : model.materials)
        {
            ImportedMaterial outMaterial = {};
            outMaterial.name = material.name;

            const tinygltf::PbrMetallicRoughness& pbr = material.pbrMetallicRoughness;

            outMaterial.baseColorTexture = GetImageIndexFromTextureIndex(model, pbr.baseColorTexture.index);
            outMaterial.metallicRoughnessTexture = GetImageIndexFromTextureIndex(model, pbr.metallicRoughnessTexture.index);
            outMaterial.normalTexture = GetImageIndexFromTextureIndex(model, material.normalTexture.index);

            if (pbr.baseColorFactor.size() == 4)
            {
                outMaterial.baseColorFactor = Freeside::Math::Vec4(
                    static_cast<float>(pbr.baseColorFactor[0]),
                    static_cast<float>(pbr.baseColorFactor[1]),
                    static_cast<float>(pbr.baseColorFactor[2]),
                    static_cast<float>(pbr.baseColorFactor[3])
                );
            }

            outMaterial.metallicFactor = static_cast<float>(pbr.metallicFactor);
            outMaterial.roughnessFactor = static_cast<float>(pbr.roughnessFactor);
            outMaterial.alphaCutoff = static_cast<float>(material.alphaCutoff);
            outMaterial.alphaMode = material.alphaMode;

            outMaterial.emissiveFactor = Freeside::Math::Vec3(
                static_cast<float>(material.emissiveFactor[0]),
                static_cast<float>(material.emissiveFactor[1]),
                static_cast<float>(material.emissiveFactor[2])
            );

            outMaterial.doubleSided = material.doubleSided;
            outModel.materials.push_back(std::move(outMaterial));
        }
    }

    ImportedModel ModelImporter::ImportModel(const std::filesystem::path& path)
	{
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        std::string error;
        std::string warning;

        bool loaded = false;

        if (path.extension() == ".glb")
        {
            loaded = loader.LoadBinaryFromFile(
                &model,
                &error,
                &warning,
                path.string()
            );
        }
        else
        {
            loaded = loader.LoadASCIIFromFile(
                &model,
                &error,
                &warning,
                path.string()
            );
        }

        if (!warning.empty())
        {
            // Log warning.
        }

        if (!loaded)
        {
            throw std::runtime_error("Failed to load glTF: " + error);
        }

        ImportedModel imported = {};

        ImportTextures(model, imported);
        ImportMaterials(model, imported);
        ImportMeshes(model, imported);

        return imported;
	}
}