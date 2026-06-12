#pragma once
#include "Entity.h"
#include "..\..\efg\include\render\types\FramePacket.h"
#include "..\..\Freeside.Assets\include\ImportedAssetTypes.h"
#include "..\..\Freeside.Platform\include\InputState.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace Freeside
{
	class Renderer;

	namespace Scene
	{

        struct SceneLoadRequest
        {
            std::filesystem::path path;
        };

        class Scene
        {
        public:
            Scene() {};
            Scene(std::wstring name);
            void Update(InputState input, const float deltaTime);
            Entity CreateEntity();
            Entity CreateEntity(std::string name);
            TransformComponent& AddTransform(Entity entity);
            MeshRendererComponent& AddMeshRenderer(Entity entity);
            CameraComponent& AddCamera(Entity entity);
            FirstPersonCameraControllerComponent& AddFirstPersonCameraControllerComponent(Entity entity);
            DirectionalLightComponent& AddDirectionalLight(Entity entity);
            PointLightComponent& AddPointLight(Entity entity);
            efg::FramePacket BuildFramePacket(uint64_t frameId) const;
            SceneEnvironment& Environment() { return m_environment; }
            HierarchyComponent& AddHierarchy(Entity entity);
            HierarchyComponent* GetHierarchy(Entity entity);
            void SetParent(Entity child, Entity newParent);
            const std::vector<Entity>& GetEntities() const;

            bool HasTransform(Entity entity) const;
            bool HasMeshRenderer(Entity entity) const;
            bool HasCamera(Entity entity) const;
            bool HasDirectionalLight(Entity entity) const;
            bool HasPointLight(Entity entity) const;

            TransformComponent* GetTransform(Entity entity);
            MeshRendererComponent* GetMeshRenderer(Entity entity);
            CameraComponent* GetCamera(Entity entity);
            DirectionalLightComponent* GetDirectionalLight(Entity entity);
            PointLightComponent* GetPointLight(Entity entity);

            bool IsValid(Entity entity) const;

            std::wstring name;

        private:
            const TransformComponent* GetTransform(Entity entity) const;
            Math::Mat4 GetWorldMatrix(Entity entity) const;
            bool IsDescendantOf(Entity possibleDescendant, Entity possibleAncestor) const;
            void BuildCamera(efg::FramePacket& packet) const;
            void BuildRenderObjects(efg::FramePacket& packet) const;
            void BuildDirectionalLights(efg::FramePacket& packet) const;
            void BuildPointLights(efg::FramePacket& packet) const;
            void BuildEnvironment(efg::FramePacket& packet) const;

        private:
            EntityId m_nextEntityId = 1;
            std::vector<Entity> m_entities;
            std::unordered_map<EntityId, TransformComponent> m_transforms;
            std::unordered_map<EntityId, MeshRendererComponent> m_meshRenderers;
            std::unordered_map<EntityId, CameraComponent> m_cameras;
            std::unordered_map<EntityId, FirstPersonCameraControllerComponent> m_firstPersonCameraControllers;
            std::unordered_map<EntityId, DirectionalLightComponent> m_directionalLights;
            std::unordered_map<EntityId, PointLightComponent> m_pointLights;
            std::unordered_map<EntityId, HierarchyComponent> m_hierarchyComponents;
            SceneEnvironment m_environment;
        };
	}
}