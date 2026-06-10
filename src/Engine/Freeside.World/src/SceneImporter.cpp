#include "..\include\SceneImporter.h"

namespace Freeside::Scene
{
    Entity SceneImporter::ImportModel(Scene& scene, Assets::AssetManager& assets, const Assets::ImportedModel& model)
    {
        Entity lastRoot = Entity::Invalid();

        for (int rootNodeIndex : model.rootNodes)
        {
            lastRoot = ImportNode(scene, assets, model, rootNodeIndex, Entity::Invalid());
        }

        return lastRoot;
    }

    Entity SceneImporter::ImportNode(Scene& scene, Assets::AssetManager& assets, const Assets::ImportedModel& model, int nodeIndex, Entity parent)
    {
        const Assets::ImportedNode& importedNode = model.nodes[nodeIndex];

        Entity entity = scene.CreateEntity();

        TransformComponent& transform = scene.AddTransform(entity);
        transform.position = importedNode.position;
        transform.rotation = importedNode.rotation;
        transform.scale = importedNode.scale;
        transform.useMatrixOverride = importedNode.useMatrixOverride;
        transform.matrixOverride = importedNode.matrixOverride;

        if (parent.IsValid())
        {
            scene.SetParent(entity, parent);
        }

        if (importedNode.meshIndex >= 0)
        {
            const Assets::ImportedMesh& importedMesh = model.meshes[importedNode.meshIndex];

            for (const Assets::ImportedPrimitive& prim : importedMesh.primitives)
            {
                MeshHandle meshHandle = assets.CreateMesh(prim.meshData);
                MaterialDesc materialDesc = assets.ConvertGLTFMaterial(model.materials[prim.materialIndex], model.textures);
                MaterialHandle materialHandle = assets.CreateMaterial(materialDesc);
                Entity primitiveEntity = scene.CreateEntity();
                scene.SetParent(primitiveEntity, entity);
                TransformComponent& primitiveTransform = scene.AddTransform(primitiveEntity);
                primitiveTransform.position = {};
                primitiveTransform.rotation = Math::Quat::Identity();
                primitiveTransform.scale = { 1.0f, 1.0f, 1.0f };
                MeshRendererComponent& renderer = scene.AddMeshRenderer(primitiveEntity);
                renderer.mesh = meshHandle;
                renderer.material = materialHandle;
            }
        }

        for (int childNodeIndex : importedNode.children)
        {
            ImportNode(scene, assets, model, childNodeIndex, entity);
        }

        return entity;
    }
}