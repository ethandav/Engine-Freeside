#pragma once
#include "Entity.h"
#include "..\..\efg\include\render\types\FramePacket.h"
#include "..\..\Freeside.Assets\include\ImportedAssetTypes.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Freeside
{
	class Renderer;
    
    namespace Assets
    {
        class AssetManager;
    }

	namespace Scene
	{
        class Scene
        {
        public:
            Scene() {};
            Scene(std::wstring name);
            Entity CreateEntity();
            TransformComponent& AddTransform(Entity entity);
            MeshRendererComponent& AddMeshRenderer(Entity entity);
            CameraComponent& AddCamera(Entity entity);
            DirectionalLightComponent& AddDirectionalLight(Entity entity);
            PointLightComponent& AddPointLight(Entity entity);

            efg::FramePacket BuildFramePacket(uint64_t frameId) const;

            SceneEnvironment& Environment() { return m_environment; }

            Freeside::Entity CreateEntityFromImportedNode(Assets::AssetManager* assets, const Assets::ImportedModel& model, int nodeIndex, Entity parent);
            HierarchyComponent& AddHierarchy(Entity entity);
            HierarchyComponent* GetHierarchy(Entity entity);
            void SetParent(Entity child, Entity newParent);

            TransformComponent* GetTransform(Entity entity);

            std::wstring name;

        private:
            const TransformComponent* GetTransform(Entity entity) const;
            Math::Mat4 GetWorldMatrix(Entity entity) const;

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
            std::unordered_map<EntityId, DirectionalLightComponent> m_directionalLights;
            std::unordered_map<EntityId, PointLightComponent> m_pointLights;
            std::unordered_map<EntityId, HierarchyComponent> m_hierarchyComponents;

            SceneEnvironment m_environment;
        };
	}
}