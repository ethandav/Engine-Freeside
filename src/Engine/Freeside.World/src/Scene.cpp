#include "..\include\Scene.h"
#include "..\..\Freeside.Assets\include\AssetManager.h"
#include "..\..\Freeside.Core\include\math\MatrixTransform.h"
#include "..\..\Freeside.Core\include\controllers\FirstPersonCameraController.h"

#include <stdexcept>
#include <algorithm>
#include <vector>

namespace Freeside
{
	namespace Scene
	{
		Scene::Scene(std::wstring name) : name(name)
		{

		}

		void Scene::Update(InputState input, const float deltaTime)
		{
			for (const auto& [entityId, cameraComponent] : m_cameras)
			{
				if (!cameraComponent.isMainCamera)
					continue;
				
				if (m_firstPersonCameraControllers.find(entityId) != m_firstPersonCameraControllers.end())
				{
					FirstPersonCameraController::Update(m_transforms[entityId], m_firstPersonCameraControllers[entityId], input, deltaTime);
				}
			}
		}

		Entity Scene::CreateEntity()
		{
			Entity entity = {};
			entity.id = m_nextEntityId++;
			m_entities.push_back(entity);
			return entity;
		}

		TransformComponent& Scene::AddTransform(Entity entity)
		{
			TransformComponent comp = {};
			m_transforms[entity.id] = comp;
			return m_transforms[entity.id];
		}

		MeshRendererComponent& Scene::AddMeshRenderer(Entity entity)
		{
			MeshRendererComponent comp = {};
			m_meshRenderers[entity.id] = comp;
			return m_meshRenderers[entity.id];
		}

		CameraComponent& Scene::AddCamera(Entity entity)
		{
			CameraComponent comp = {};
			m_cameras[entity.id] = comp;
			return m_cameras[entity.id];
		}

		FirstPersonCameraControllerComponent& Scene::AddFirstPersonCameraControllerComponent(Entity entity)
		{
			FirstPersonCameraControllerComponent comp = {};

			TransformComponent* transform = GetTransform(entity);
			if (!transform)
			{
				throw std::runtime_error("FirstPersonCameraController requires TransformComponent.");
			}

			Math::Vec3 forward = Math::RotateVector(m_transforms[entity.id].rotation, Math::Vec3(0.0f, 0.0f, 1.0f));
			forward = Math::Normalize(forward);
			comp.pitch = std::asin(forward.y);
			comp.yaw = std::atan2(forward.x, forward.z);

			m_firstPersonCameraControllers[entity.id] = comp;
			return m_firstPersonCameraControllers[entity.id];
		}

		DirectionalLightComponent& Scene::AddDirectionalLight(Entity entity)
		{
			DirectionalLightComponent comp = {};
			m_directionalLights[entity.id] = comp;
			return m_directionalLights[entity.id];
		}

		PointLightComponent& Scene::AddPointLight(Entity entity)
		{
			PointLightComponent comp = {};
			m_pointLights[entity.id] = comp;
			return m_pointLights[entity.id];
		}

		efg::FramePacket Scene::BuildFramePacket(uint64_t frameId) const
		{
			efg::FramePacket packet = {};
			packet.frameId = frameId;

			BuildCamera(packet);
			BuildRenderObjects(packet);
			BuildDirectionalLights(packet);
			BuildPointLights(packet);
			BuildEnvironment(packet);

			return packet;
		}

		void Scene::BuildCamera(efg::FramePacket& packet) const
		{
			for (const auto& [entityId, cameraComponent] : m_cameras)
			{
				if (!cameraComponent.isMainCamera)
					continue;

				auto transformIt = m_transforms.find(entityId);
				if (transformIt == m_transforms.end())
					continue;

				const TransformComponent& transform = transformIt->second;

				Math::Vec3 forward = Math::RotateVector(transform.rotation, Math::Vec3(0.0f, 0.0f, 1.0f));
				Math::Vec3 up = Math::RotateVector(transform.rotation, Math::Vec3(0.0f, 1.0f, 0.0f));
				Math::Vec3 right = Math::RotateVector(transform.rotation, Math::Vec3(1.0f, 0.0f, 0.0f));

				packet.camera.LookAt(transform.position, transform.position + forward, up);
				packet.camera.SetPosition(transform.position);

				return;
			}

			// Optional fallback camera if no main camera exists.
			packet.camera = {};
		}

		void Scene::BuildRenderObjects(efg::FramePacket& packet) const
		{
			packet.renderObjects.clear();

			for (const auto& [entityId, meshRenderer] : m_meshRenderers)
			{
				Entity entity{ entityId };

				const TransformComponent* transform = GetTransform(entity);
				if (!transform)
				{
					continue;
				}

				Freeside::RenderObject object = {};
				object.mesh = meshRenderer.mesh;
				object.material = meshRenderer.material;
				object.world = GetWorldMatrix(entity);

				packet.renderObjects.push_back(object);
			}
		}

		void Scene::BuildDirectionalLights(efg::FramePacket& packet) const
		{
			packet.directionalLights.clear();

			for (const auto& [entityId, light] : m_directionalLights)
			{
				Freeside::Lights::Directional out = {};
				out.direction = light.direction;
				out.color = light.color;
				out.intensity = light.intensity;

				packet.directionalLights.push_back(out);
			}
		}

		void Scene::BuildPointLights(efg::FramePacket& packet) const
		{
			packet.pointLights.clear();

			for (const auto& [entityId, light] : m_pointLights)
			{
				auto transformIt = m_transforms.find(entityId);
				if (transformIt == m_transforms.end())
					continue;

				const TransformComponent& transform = transformIt->second;

				Freeside::Lights::Point out = {};
				out.position = transform.position;
				out.color = light.color;
				out.intensity = light.intensity;
				out.radius = light.radius;

				packet.pointLights.push_back(out);
			}
		}

		void Scene::BuildEnvironment(efg::FramePacket& packet) const
		{
			packet.skyboxTexture = m_environment.skyboxTexture;
		}

		Entity Scene::CreateEntityFromImportedNode(Assets::AssetManager* assets, const Assets::ImportedModel& model, int nodeIndex, Entity parent)
		{
			const Assets::ImportedNode& importedNode = model.nodes[nodeIndex];

			Entity entity = CreateEntity();

			TransformComponent& transform = AddTransform(entity);
			transform.position = importedNode.position;
			transform.rotation = importedNode.rotation;
			transform.scale = importedNode.scale;
			transform.useMatrixOverride = importedNode.useMatrixOverride;
			transform.matrixOverride = importedNode.matrixOverride;

			if (parent.IsValid())
			{
				SetParent(entity, parent);
			}

			if (importedNode.meshIndex >= 0)
			{
				const Assets::ImportedMesh& importedMesh = model.meshes[importedNode.meshIndex];

				for (const Assets::ImportedPrimitive& prim : importedMesh.primitives)
				{
					MeshHandle meshHandle = assets->CreateMesh(prim.meshData);
					MaterialDesc importMat = assets->ConvertGLTFMaterial(model.materials[prim.materialIndex], model.textures);
					Freeside::MaterialHandle materialHandle = assets->CreateMaterial(importMat);
					Freeside::Entity primitiveEntity = CreateEntity();
					SetParent(primitiveEntity, entity);

					TransformComponent& primitiveTransform = AddTransform(primitiveEntity);
					primitiveTransform.position = {};
					primitiveTransform.rotation = Freeside::Math::Quat::Identity();
					primitiveTransform.scale = { 1.0f, 1.0f, 1.0f };

					Freeside::MeshRendererComponent& renderer = AddMeshRenderer(primitiveEntity);
					renderer.mesh = meshHandle;
					renderer.material = materialHandle;
				}
			}

			for (int childNodeIndex : importedNode.children)
			{
				CreateEntityFromImportedNode(assets, model, childNodeIndex, entity);
			}

			return entity;
		}

		HierarchyComponent& Scene::AddHierarchy(Entity entity)
		{
			return m_hierarchyComponents[entity.id];
		}

		HierarchyComponent* Scene::GetHierarchy(Entity entity)
		{
			auto it = m_hierarchyComponents.find(entity.id);

			if (it == m_hierarchyComponents.end())
			{
				return nullptr;
			}

			return &it->second;
		}

		TransformComponent* Scene::GetTransform(Entity entity)
		{
			auto it = m_transforms.find(entity.id);

			if (it == m_transforms.end())
			{
				return nullptr;
			}

			return &it->second;
		}

		const TransformComponent* Scene::GetTransform(Entity entity) const
		{
			auto it = m_transforms.find(entity.id);

			if (it == m_transforms.end())
			{
				return nullptr;
			}

			return &it->second;
		}

		void Scene::SetParent(Entity child, Entity newParent)
		{
			if (!child.IsValid())
			{
				return;
			}

			if (child == newParent)
			{
				return;
			}

			// Prevent cycles:
			// You cannot parent an entity to one of its own descendants.
			if (newParent.IsValid() && IsDescendantOf(newParent, child))
			{
				return;
			}

			HierarchyComponent& childHierarchy = m_hierarchyComponents[child.id];

			Entity oldParent = childHierarchy.parent;

			if (oldParent.IsValid())
			{
				HierarchyComponent* oldParentHierarchy = GetHierarchy(oldParent);

				if (oldParentHierarchy)
				{
					std::vector<Entity>& siblings = oldParentHierarchy->children;

					siblings.erase(
						std::remove(siblings.begin(), siblings.end(), child),
						siblings.end()
					);
				}
			}

			childHierarchy.parent = newParent;

			if (newParent.IsValid())
			{
				HierarchyComponent& newParentHierarchy = m_hierarchyComponents[newParent.id];

				auto it = std::find(
					newParentHierarchy.children.begin(),
					newParentHierarchy.children.end(),
					child
				);

				if (it == newParentHierarchy.children.end())
				{
					newParentHierarchy.children.push_back(child);
				}
			}
		}

		Math::Mat4 Scene::GetWorldMatrix(Entity entity) const
		{
			const TransformComponent* transform = GetTransform(entity);

			if (!transform)
				return Math::Mat4::Identity();

			Math::Mat4 local = transform->GetLocalMatrix();

			auto hierarchyIt = m_hierarchyComponents.find(entity.id);

			if (hierarchyIt == m_hierarchyComponents.end())
			{
				return local;
			}

			Entity parent = hierarchyIt->second.parent;

			if (!parent.IsValid())
			{
				return local;
			}

			return GetWorldMatrix(parent) * local;
		}

		bool Scene::IsDescendantOf(Entity possibleDescendant, Entity possibleAncestor) const
		{
			if (!possibleDescendant.IsValid() || !possibleAncestor.IsValid())
			{
				return false;
			}

			Entity current = possibleDescendant;

			while (current.IsValid())
			{
				if (current == possibleAncestor)
				{
					return true;
				}

				auto hierarchyIt = m_hierarchyComponents.find(current.id);

				if (hierarchyIt == m_hierarchyComponents.end())
				{
					return false;
				}

				current = hierarchyIt->second.parent;
			}

			return false;
		}
	}
}
