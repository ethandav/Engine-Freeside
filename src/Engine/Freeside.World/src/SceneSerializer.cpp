#include "..\include\SceneSerializer.h"
#include "..\..\Freeside.Core\include\math\Quat.h"
#include "..\..\Freeside.Core\include\math\Vec3.h"
#include "..\..\Freeside.Assets\include\ImportedAssetTypes.h"
#include "..\include\SceneImporter.h"
#include "..\..\Freeside.Core\include\shapes\shapes.h"

#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <Windows.h>

namespace Freeside::Scene
{

    static std::wstring ToWide(const std::string& text)
    {
        if (text.empty())
        {
            return {};
        }

        int requiredSize = MultiByteToWideChar(
            CP_UTF8,
            0,
            text.data(),
            static_cast<int>(text.size()),
            nullptr,
            0
        );

        if (requiredSize <= 0)
        {
            return {};
        }

        std::wstring result(requiredSize, L'\0');

        MultiByteToWideChar(
            CP_UTF8,
            0,
            text.data(),
            static_cast<int>(text.size()),
            result.data(),
            requiredSize
        );

        return result;
    }

    static Math::Vec3 ReadVec3(const nlohmann::json& vecJson)
    {
        return Math::Vec3(
            vecJson[0].get<float>(),
            vecJson[1].get<float>(),
            vecJson[2].get<float>()
        );
    }

    static Math::Quat ReadQuat(const nlohmann::json& quatJson)
    {
        return Math::Quat(
            quatJson[0].get<float>(),
            quatJson[1].get<float>(),
            quatJson[2].get<float>(),
            quatJson[3].get<float>()
        );
    }

    static Math::Vec2 ReadVec2(const nlohmann::json& j)
    {
        return Math::Vec2(
            j[0].get<float>(),
            j[1].get<float>()
        );
    }

    static Math::Vec4 ReadVec4(const nlohmann::json& j)
    {
        return Math::Vec4(
            j[0].get<float>(),
            j[1].get<float>(),
            j[2].get<float>(),
            j[3].get<float>()
        );
    }

    static void DeserializeTransform(TransformComponent& transform, const nlohmann::json& transformJson)
    {
        transform.position = ReadVec3(transformJson["position"]);
        if (transformJson.contains("rotation"))
            transform.rotation = ReadQuat(transformJson["rotation"]);
        else
            transform.rotation = Math::Quat::Identity();
        transform.scale = ReadVec3(transformJson["scale"]);

        if (transformJson.contains("userMatrixOverride"))
            transform.useMatrixOverride = transformJson["userMatrixOverride"].get<bool>();
    }

    static void DeserializeCamera(CameraComponent& camera, const nlohmann::json& cameraJson)
    {
        camera.farZ = cameraJson["farZ"].get<float>();
        camera.fovYRadians = cameraJson["fovYRadians"].get<float>();
        camera.isMainCamera = cameraJson["isMainCamera"].get<bool>();
        camera.nearZ = cameraJson["nearZ"].get<float>();
        camera.orthographicHeight = cameraJson["orthographicHeight"].get<float>();
    }

    static void DeserializefirstPersonCameraController(FirstPersonCameraControllerComponent& camera, const nlohmann::json& cameraControllerJson)
    {
        camera.lookSensitivity = cameraControllerJson["lookSensitivity"].get<float>();
        camera.moveSpeed = cameraControllerJson["moveSpeed"].get<float>();
        camera.allowVerticalMovement = cameraControllerJson["allowVerticalMovement"].get<bool>();
    }

    static void DeserializePointLight(PointLightComponent& pointLight, const nlohmann::json& pointLightJson)
    {
        pointLight.color = ReadVec3(pointLightJson["color"]);
        pointLight.intensity = pointLightJson["intensity"].get<float>();
        pointLight.radius = pointLightJson["radius"].get<float>();
    }

    static void DeserializeDirectionalLight(DirectionalLightComponent& directionalLight, const nlohmann::json& directionalLightJson)
    {
        directionalLight.color = ReadVec3(directionalLightJson["color"]);
        directionalLight.intensity = directionalLightJson["intensity"].get<float>();
        directionalLight.direction = ReadVec3(directionalLightJson["direction"]);
    }

    static MeshHandle DeserializeMeshRef(const nlohmann::json& meshJson, Assets::AssetManager& assets)
    {
        std::string type = meshJson["type"].get<std::string>();

        if (type == "builtin")
        {
            std::string name = meshJson["name"].get<std::string>();

            if (name == "cube")
                return assets.CreateMesh(Shapes::Cube().mesh);

            if (name == "plane")
                return assets.CreateMesh(Shapes::Plane().mesh);

            if (name == "wall")
                return assets.CreateMesh(Shapes::Wall().mesh);

            throw std::runtime_error("Unknown builtin mesh: " + name);
        }

        throw std::runtime_error("Unknown mesh type: " + type);
    }

    static MaterialHandle DeserializeMaterial(
        const nlohmann::json& materialJson,
        Assets::AssetManager& assets)
    {
        MaterialDesc desc = {};

        if (materialJson.contains("baseColorFactor"))
            desc.baseColorFactor = ReadVec4(materialJson["baseColorFactor"]);
        else
            desc.baseColorFactor = Math::Vec4(1, 1, 1, 1);

        if (materialJson.contains("metallicFactor"))
            desc.metallicFactor = materialJson["metallicFactor"].get<float>();

        if (materialJson.contains("roughnessFactor"))
            desc.roughnessFactor = materialJson["roughnessFactor"].get<float>();

        if (materialJson.contains("specular"))
            desc.specular = ReadVec2(materialJson["specular"]);

        if (materialJson.contains("uvScale"))
            desc.uvScale = ReadVec2(materialJson["uvScale"]);
        else
            desc.uvScale = Math::Vec2(1.0f, 1.0f);

        if (materialJson.contains("textures"))
        {
            const nlohmann::json& texturesJson = materialJson["textures"];

            if (texturesJson.contains("baseColor"))
            {
                desc.baseColorTexture =
                    assets.CreateTextureFromImagePath(
                        ToWide(texturesJson["baseColor"].get<std::string>())
                    );
            }

            if (texturesJson.contains("normal"))
            {
                desc.normalTexture =
                    assets.CreateTextureFromImagePath(
                        ToWide(texturesJson["normal"].get<std::string>())
                    );
            }

            if (texturesJson.contains("height"))
            {
                desc.heightTexture =
                    assets.CreateTextureFromImagePath(
                        ToWide(texturesJson["height"].get<std::string>())
                    );
            }

            if (texturesJson.contains("metallicRoughness"))
            {
                desc.metallicRoughnessTexture =
                    assets.CreateTextureFromImagePath(
                        ToWide(texturesJson["metallicRoughness"].get<std::string>())
                    );
            }

            if (texturesJson.contains("ao"))
            {
                desc.occlusionTexture =
                    assets.CreateTextureFromImagePath(
                        ToWide(texturesJson["ao"].get<std::string>())
                    );
            }

            if (texturesJson.contains("emissive"))
            {
                desc.emissiveTexture =
                    assets.CreateTextureFromImagePath(
                        ToWide(texturesJson["emissive"].get<std::string>())
                    );
            }
        }

        return assets.CreateMaterial(desc);
    }

    static bool HasComponent(const nlohmann::json& entityJson, const char* componentName)
    {
        auto componentsIt = entityJson.find("components");

        if (componentsIt == entityJson.end() || !componentsIt->is_object())
            return false;

        return componentsIt->contains(componentName);
    }

    bool SceneSerializer::Load(Scene& scene, const std::filesystem::path& path, Assets::AssetManager& assets)
    {
        using json = nlohmann::json;

        std::ifstream file(path);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open scene file: " + path.string());
        }

        nlohmann::json sceneJson;
        file >> sceneJson;

        json environmentJson = sceneJson["environment"];
        json skyboxJson = environmentJson["skyboxTextures"];

        std::array<std::wstring, 6> skyboxImages =
        {
            ToWide(skyboxJson[0].get<std::string>()),
            ToWide(skyboxJson[1].get<std::string>()),
            ToWide(skyboxJson[2].get<std::string>()),
            ToWide(skyboxJson[3].get<std::string>()),
            ToWide(skyboxJson[4].get<std::string>()),
            ToWide(skyboxJson[5].get<std::string>())
        };

        scene.Environment().skyboxTexture = assets.CreateSkyboxFromImagePaths(skyboxImages);

        std::unordered_map<uint32_t, Entity> entityMap;

        for (const auto& entityJson : sceneJson["entities"])
        {
            uint32_t serializedId = entityJson["id"];
            Entity entity = scene.CreateEntity(entityJson["name"].get<std::string>());
            entityMap[serializedId] = entity;
        }

        for (const auto& entityJson : sceneJson["entities"])
        {
            Entity entity = entityMap[entityJson["id"]];

            if (HasComponent(entityJson, "transform"))
            {
                TransformComponent& transform = scene.AddTransform(entity);
                DeserializeTransform(transform, entityJson["components"]["transform"]);
            }

            if (HasComponent(entityJson, "importModel"))
            {
                const auto& importJson = entityJson["components"]["importModel"];

                std::string path = importJson["path"].get<std::string>();

                Assets::ImportedModel model = assets.ImportModel(path);

                SceneImporter::ImportModel(
                    scene,
                    assets,
                    model,
                    entity
                );
            }

            if (HasComponent(entityJson, "camera"))
            {
                CameraComponent& camera = scene.AddCamera(entity);
                DeserializeCamera(camera, entityJson["components"]["camera"]);
            }

            if (HasComponent(entityJson, "firstPersonCameraController"))
            {
                FirstPersonCameraControllerComponent& camera = scene.AddFirstPersonCameraControllerComponent(entity);
                DeserializefirstPersonCameraController(camera, entityJson["components"]["firstPersonCameraController"]);
            }

            if (HasComponent(entityJson, "pointLight"))
            {
                PointLightComponent& light = scene.AddPointLight(entity);
                DeserializePointLight(light, entityJson["components"]["pointLight"]);
            }

            if (HasComponent(entityJson, "directionalLight"))
            {
                DirectionalLightComponent& light = scene.AddDirectionalLight(entity);
                DeserializeDirectionalLight(light, entityJson["components"]["directionalLight"]);
            }

            if (HasComponent(entityJson, "meshRenderer"))
            {
                const json& meshRendererJson = entityJson["components"]["meshRenderer"];

                MeshRendererComponent& renderer = scene.AddMeshRenderer(entity);

                renderer.mesh = DeserializeMeshRef(meshRendererJson["mesh"], assets);
                renderer.material = DeserializeMaterial(meshRendererJson["material"], assets);
            }
        }

		return true;
	}
}