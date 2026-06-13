#include "..\include\EditorInterface.h"
#include "..\..\..\Engine\Freeside.Platform\include\FileDialog.h"

namespace Freeside::Editor
{
	void EditorInterface::Initialize(Renderer* renderer, void* hwnd)
	{
		m_renderer = renderer;
		m_ownerWindow = hwnd;
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
			if (!scene.HasHirearchy(entity) || !scene.GetHirearchy(entity)->parent.IsValid())
			{
				DrawEntityNode(scene, entity);
			}
		}

		ImGui::End();
	}

	void EditorInterface::DrawEntityNode(Scene::Scene& scene, Freeside::Entity entity)
	{
		const bool selected =
			m_state.hasSelectedEntity &&
			m_state.selectedEntity.id == entity.id;

		const HierarchyComponent* hierarchy = scene.GetHierarchy(entity);

		const bool hasChildren =
			hierarchy != nullptr &&
			!hierarchy->children.empty();

		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		if (!hasChildren)
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
			flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		std::string label = entity.name.empty()
			? "Entity " + std::to_string(entity.id)
			: entity.name;

		bool open = ImGui::TreeNodeEx(
			reinterpret_cast<void*>(static_cast<uintptr_t>(entity.id)),
			flags,
			"%s",
			label.c_str()
		);

		if (ImGui::IsItemClicked())
		{
			m_state.selectedEntity = entity;
			m_state.hasSelectedEntity = true;
		}

		if (hasChildren && open)
		{
			for (Freeside::Entity child : hierarchy->children)
			{
				DrawEntityNode(scene, child);
			}

			ImGui::TreePop();
		}
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
			bool valueChanged = false;


			static char baseColorPath[260] = {};
			static char normalPath[260] = {};
			static char metallicRoughnessPath[260] = {};

			ImGui::Text("Base Color");

			if (material->baseColorTexture.IsValid())
			{
				uint64_t textureId = assets->GetTextureID(material->baseColorTexture);

				if(ImGui::ImageButton("BaseColorImageButton", static_cast<ImTextureID>(textureId), ImVec2(64.0f, 64.0f)))
				{
					auto path = Freeside::OpenFileDialog(m_ownerWindow,
						L"Image Files\0*.png;*.jpg;*.jpeg;*.tga;*.bmp;*.dds\0All Files\0*.*\0"
					);

					if (path)
					{
						material->baseColorTexture = assets->CreateTextureFromImagePath(*path);
						assets->UpdateMaterial(materialHandle);
					}
				}
			}
			else
			{
				if (ImGui::Button("Empty", ImVec2(64.0f, 64.0f)))
				{
					auto path = Freeside::OpenFileDialog(m_ownerWindow,
						L"Image Files\0*.png;*.jpg;*.jpeg;*.tga;*.bmp;*.dds\0All Files\0*.*\0"
					);

					if (path)
					{
						material->baseColorTexture = assets->CreateTextureFromImagePath(*path);
						assets->UpdateMaterial(materialHandle);
					}
				}
			}

			ImGui::SameLine();

			valueChanged |= ImGui::ColorEdit4("Base Color", &material->baseColorFactor.x);

			ImGui::Text("Normal Map");

			if (material->normalTexture.IsValid())
			{
				uint64_t textureId = assets->GetTextureID(material->normalTexture);

				if (ImGui::ImageButton("NormalMapImageButton", static_cast<ImTextureID>(textureId), ImVec2(64.0f, 64.0f)))
				{
					auto path = Freeside::OpenFileDialog(m_ownerWindow,
						L"Image Files\0*.png;*.jpg;*.jpeg;*.tga;*.bmp;*.dds\0All Files\0*.*\0"
					);

					if (path)
					{
						material->normalTexture = assets->CreateTextureFromImagePath(*path);
						assets->UpdateMaterial(materialHandle);
					}
				}
			}
			else
			{
				if (ImGui::Button("Empty", ImVec2(64.0f, 64.0f)))
				{
					auto path = Freeside::OpenFileDialog(m_ownerWindow,
						L"Image Files\0*.png;*.jpg;*.jpeg;*.tga;*.bmp;*.dds\0All Files\0*.*\0"
					);

					if (path)
					{
						material->normalTexture = assets->CreateTextureFromImagePath(*path);
						assets->UpdateMaterial(materialHandle);
					}
				}
			}

			ImGui::SameLine();

			valueChanged |= ImGui::DragFloat("Normal Scale", &material->normalScale, 0.01f, 0.0f, 2.0f);

			ImGui::Text("Height Map");

			if (material->heightTexture.IsValid())
			{
				uint64_t textureId = assets->GetTextureID(material->heightTexture);

				if (ImGui::ImageButton("HeightMapImageButton", static_cast<ImTextureID>(textureId), ImVec2(64.0f, 64.0f)))
				{
					auto path = Freeside::OpenFileDialog(m_ownerWindow,
						L"Image Files\0*.png;*.jpg;*.jpeg;*.tga;*.bmp;*.dds\0All Files\0*.*\0"
					);

					if (path)
					{
						material->heightTexture = assets->CreateTextureFromImagePath(*path);
						assets->UpdateMaterial(materialHandle);
					}
				}
			}
			else
			{
				if (ImGui::Button("Empty", ImVec2(64.0f, 64.0f)))
				{
					auto path = Freeside::OpenFileDialog(m_ownerWindow,
						L"Image Files\0*.png;*.jpg;*.jpeg;*.tga;*.bmp;*.dds\0All Files\0*.*\0"
					);

					if (path)
					{
						material->heightTexture = assets->CreateTextureFromImagePath(*path);
						assets->UpdateMaterial(materialHandle);
					}
				}
			}

			ImGui::SameLine();

			valueChanged |= ImGui::DragFloat("Height Scale", &material->heightScale, 0.01f, 0.0f, 0.10f);

			ImGui::Text("Metallic and Roughness Texture");

			if (material->metallicRoughnessTexture.IsValid())
			{
				uint64_t textureId = assets->GetTextureID(material->metallicRoughnessTexture);

				if (ImGui::ImageButton("MetalicRoughnessTextureImageButton", static_cast<ImTextureID>(textureId), ImVec2(64.0f, 64.0f)))
				{
					auto path = Freeside::OpenFileDialog(m_ownerWindow,
						L"Image Files\0*.png;*.jpg;*.jpeg;*.tga;*.bmp;*.dds\0All Files\0*.*\0"
					);

					if (path)
					{
						material->metallicRoughnessTexture = assets->CreateTextureFromImagePath(*path);
						assets->UpdateMaterial(materialHandle);
					}
				}
			}
			else
			{
				if (ImGui::Button("Empty", ImVec2(64.0f, 64.0f)))
				{
					auto path = Freeside::OpenFileDialog(m_ownerWindow,
						L"Image Files\0*.png;*.jpg;*.jpeg;*.tga;*.bmp;*.dds\0All Files\0*.*\0"
					);

					if (path)
					{
						material->metallicRoughnessTexture = assets->CreateTextureFromImagePath(*path);
						assets->UpdateMaterial(materialHandle);
					}
				}
			}

			ImGui::SameLine();

			ImGui::BeginGroup();
			valueChanged |= ImGui::DragFloat("Metallic", &material->metallicFactor, 0.01f, 0.0f, 1.0f);
			valueChanged |= ImGui::DragFloat("Roughness", &material->roughnessFactor, 0.01f, 0.0f, 1.0f);
			ImGui::EndGroup();

			ImGui::TreePop();

			if (valueChanged)
				assets->UpdateMaterial(materialHandle);
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