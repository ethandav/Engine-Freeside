#include "..\include\SceneSerializer.h"
#include "..\..\Freeside.Core\include\math\Quat.h"
#include "..\..\Freeside.Core\include\math\Vec3.h"
#include "..\..\Freeside.Assets\include\ImportedAssetTypes.h"
#include "..\include\SceneImporter.h"

#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

namespace Freeside::Scene
{
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

        std::unordered_map<uint32_t, Entity> entityMap;

        for (const auto& entityJson : sceneJson["entities"])
        {
            uint32_t serializedId = entityJson["id"];
            Entity entity = scene.CreateEntity();
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
        }

		return true;
	}
}