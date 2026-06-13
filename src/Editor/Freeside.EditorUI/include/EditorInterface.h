#pragma once
#include "..\..\..\Engine\efg\include\render\Renderer.h"
#include "..\..\..\Editor\Freeside.EditorUI\imgui\imgui.h"
#include "..\..\..\Engine\Freeside.World\include\Scene.h"
#include "..\..\..\Engine\Freeside.World\include\SceneManager.h"
#include "..\..\..\Engine\Freeside.World\include\Entity.h"

namespace Freeside::Editor
{
    class EditorInterface
    {
    public:
        void Initialize(Renderer* renderer);
        void Draw(Scene::SceneManager& sceneManager);

    private:
        void DrawEntityHierarchy(Scene::Scene& scene);
        void DrawInspector(Scene::SceneManager& sceneManager, Scene::Scene& scene);
        void DrawAddComponentMenu(Scene::Scene& scene, Entity entity);
        void DrawTransformComponent(TransformComponent* transform);
        void DrawMeshRendererComponent(Scene::SceneManager& sceneManager, MeshRendererComponent* meshRenderer);
        void DrawMaterialEditor(Assets::AssetManager* assets, MaterialHandle materialHandle);
        void DrawPointLightComponent(PointLightComponent* light);
        void DrawDirectionalLightComponent(DirectionalLightComponent* light);
        void DrawMainMenuBar(Freeside::Scene::SceneManager& sceneManager);

        struct EditorState
        {
            Entity selectedEntity = {};
            bool hasSelectedEntity = false;
        };

        Renderer* m_renderer = nullptr;
        EditorState m_state;
    };
}
