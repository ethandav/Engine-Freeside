#include "..\include\EditorInterface.h"

namespace Freeside::Editor
{
	void EditorInterface::Initialize(Renderer* renderer)
	{
		m_renderer = renderer;
	}

	void EditorInterface::Draw(Scene::SceneManager& sceneManager)
	{
		DrawMainMenuBar(sceneManager);

		Freeside::Scene::Scene* scene = sceneManager.GetActiveScene();

		if (!scene)
			return;

		DrawEntityHierarchy(*scene);
		DrawInspector(sceneManager, *scene);
	}

	void EditorInterface::DrawEntityHierarchy(Scene::Scene& scene)
	{
		ImGui::Begin("Entity Hierarchy");

		if (ImGui::Button("+ Entity"))
		{
			Freeside::Entity entity = scene.CreateEntity();
			scene.AddTransform(entity);

			m_state.selectedEntity = entity;
			m_state.hasSelectedEntity = true;
		}

		ImGui::Separator();

		for (Freeside::Entity entity : scene.GetEntities())
		{
			std::string label = "Entity " + entity.name;

			bool selected = m_state.hasSelectedEntity && m_state.selectedEntity.id == entity.id;

			if (ImGui::Selectable(label.c_str(), selected))
			{
				m_state.selectedEntity = entity;
				m_state.hasSelectedEntity = true;
			}
		}

		ImGui::End();
	}

	void EditorInterface::DrawInspector(Scene::SceneManager& sceneManager, Scene::Scene& scene)
	{
		ImGui::Begin("Inspector");

		if (!m_state.hasSelectedEntity || !scene.IsValid(m_state.selectedEntity))
		{
			ImGui::Text("No entity selected.");
			ImGui::End();
			return;
		}

		Freeside::Entity entity = m_state.selectedEntity;

		ImGui::Text("Entity %u", entity.name.c_str());

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}

		DrawAddComponentMenu(scene, entity);

		ImGui::Separator();

		if (scene.HasTransform(entity))
		{
			DrawTransformComponent(scene.GetTransform(entity));
		}

		if (scene.HasMeshRenderer(entity))
		{
			DrawMeshRendererComponent(sceneManager, scene.GetMeshRenderer(entity));
		}

		if (scene.HasCamera(entity))
		{
			//DrawCameraComponent(scene.GetCamera(entity));
		}

		if (scene.HasDirectionalLight(entity))
		{
			DrawDirectionalLightComponent(scene.GetDirectionalLight(entity));
		}

		if (scene.HasPointLight(entity))
		{
			DrawPointLightComponent(scene.GetPointLight(entity));
		}

		ImGui::End();
	}

	void EditorInterface::DrawTransformComponent(TransformComponent* transform)
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Position", &transform->position.x, 0.05f);
			ImGui::DragFloat3("Scale", &transform->scale.x, 0.05f, 0.001f, 1000.0f);

			// Only do this if rotation is Vec3 Euler.
			// ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.01f);
		}
	}

	void EditorInterface::DrawMeshRendererComponent(Scene::SceneManager& sceneManager, MeshRendererComponent* meshRenderer)
	{
		if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (!meshRenderer->material.IsValid())
			{
				if (ImGui::Button("Create Default Material"))
				{
					meshRenderer->material = sceneManager.Assets()->CreateDefaultMaterial();
				}
			}
			else
			{
				DrawMaterialEditor(sceneManager.Assets(), meshRenderer->material);
			}
		}
	}

	void EditorInterface::DrawMaterialEditor(Assets::AssetManager* assets, MaterialHandle materialHandle)
	{
		MaterialDesc* material = assets->GetRegisteredMaterial(materialHandle);

		if (ImGui::TreeNode("Material"))
		{
			ImGui::ColorEdit4("Base Color", &material->baseColorFactor.x);
			ImGui::DragFloat("Metallic", &material->metallicFactor, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Roughness", &material->roughnessFactor, 0.01f, 0.0f, 1.0f);

			static char baseColorPath[260] = {};
			static char normalPath[260] = {};
			static char metallicRoughnessPath[260] = {};

			ImGui::InputText("Base Color Texture", baseColorPath, sizeof(baseColorPath));
			if (ImGui::Button("Load Base Color"))
			{
				//material->baseColorTexture = assets->LoadTexture(baseColorPath, TextureUsage::BaseColor);
			}

			ImGui::InputText("Normal Texture", normalPath, sizeof(normalPath));
			if (ImGui::Button("Load Normal"))
			{
				//material->normalTexture = assets->LoadTexture(normalPath, TextureUsage::Normal);
			}

			ImGui::InputText("Metallic Roughness Texture", metallicRoughnessPath, sizeof(metallicRoughnessPath));
			if (ImGui::Button("Load Metallic Roughness"))
			{
				//material->metallicRoughnessTexture = assets.LoadTexture(metallicRoughnessPath, TextureUsage::MetallicRoughness);
			}

			if (ImGui::Button("Apply Material"))
			{
				assets->UpdateMaterial(materialHandle);
			}

			ImGui::TreePop();
		}
	}

	void EditorInterface::DrawPointLightComponent(PointLightComponent* light)
	{
		if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Color", &light->color.x, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Radius", &light->radius, 0.1f, 0.0f, 1000.0f);
		}
	}

	void EditorInterface::DrawDirectionalLightComponent(DirectionalLightComponent* light)
	{
		if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Direction", &light->direction.x, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat3("Color", &light->color.x, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 1000.0f);
		}
	}

	void EditorInterface::DrawAddComponentMenu(Freeside::Scene::Scene& scene, Freeside::Entity entity)
	{
		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			if (!scene.HasTransform(entity))
			{
				if (ImGui::MenuItem("Transform"))
				{
					scene.AddTransform(entity);
				}
			}

			if (!scene.HasMeshRenderer(entity))
			{
				if (ImGui::MenuItem("Mesh Renderer"))
				{
					scene.AddMeshRenderer(entity);
				}
			}

			if (!scene.HasCamera(entity))
			{
				if (ImGui::MenuItem("Camera"))
				{
					scene.AddCamera(entity);
				}
			}

			if (!scene.HasDirectionalLight(entity))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					scene.AddDirectionalLight(entity);
				}
			}

			if (!scene.HasPointLight(entity))
			{
				if (ImGui::MenuItem("Point Light"))
				{
					scene.AddPointLight(entity);
				}
			}

			ImGui::EndPopup();
		}
	}

	void EditorInterface::DrawMainMenuBar(Freeside::Scene::SceneManager& sceneManager)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Blank Scene"))
				{
					sceneManager.CreateBlankScene();
					m_state.hasSelectedEntity = false;
				}

				if (ImGui::MenuItem("Save Scene"))
				{
					sceneManager.SaveActiveScene("scenes/editor_scene.json");
				}

				if (ImGui::MenuItem("Load Scene"))
				{
					sceneManager.LoadScene("scenes/editor_scene.json");
					m_state.hasSelectedEntity = false;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}