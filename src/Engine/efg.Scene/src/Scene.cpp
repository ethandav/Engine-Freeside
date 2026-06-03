#include "..\include\Scene.h"

#include <stdexcept>

namespace Freeside
{
	namespace Scene
	{
		Scene::Scene(std::wstring name) : name(name)
		{
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

				//auto transformIt = m_transforms.find(entityId);
				//if (transformIt == m_transforms.end())
				//	continue;

				packet.camera = cameraComponent.camera;

				//const TransformComponent& transform = transformIt->second;
				//packet.camera.SetPosition(transform.position);

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
				auto transformIt = m_transforms.find(entityId);
				if (transformIt == m_transforms.end())
					continue;

				const TransformComponent& transform = transformIt->second;

				Freeside::RenderObject object = {};
				object.mesh = meshRenderer.mesh;
				object.material = meshRenderer.material;
				object.world = transform.GetWorldMatrix();

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
	}
}
