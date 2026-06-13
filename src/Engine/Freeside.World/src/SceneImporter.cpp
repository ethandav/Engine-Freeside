#include "..\include\SceneImporter.h"

namespace Freeside::Scene
{
    std::vector<Entity> SceneImporter::ImportModel(Scene& scene, Assets::AssetManager& assets, const Assets::ImportedModel& model,Entity parent)
    {
        std::vector<Entity> roots;

        for (int rootNodeIndex : model.rootNodes)
        {
            Entity root = ImportNode(
                scene,
                assets,
                model,
                rootNodeIndex,
                parent
            );

            roots.push_back(root);
        }

        return roots;
    }

    Entity SceneImporter::ImportNode(Scene& scene, Assets::AssetManager& assets, const Assets::ImportedModel& model, int nodeIndex, Entity parent)
    {
        const Assets::ImportedNode& importedNode = model.nodes[nodeIndex];

        std::string nodeName = importedNode.name.empty() ? "Imported Mesh Node" : importedNode.name;

        Entity entity = scene.CreateEntity(nodeName);

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
            int primIndex = 0;

            for (const Assets::ImportedPrimitive& prim : importedMesh.primitives)
            {
                MeshHandle meshHandle = assets.CreateMesh(prim.meshData);
                MaterialDesc materialDesc = assets.ConvertGLTFMaterial(model.materials[prim.materialIndex], model.textures);
                MaterialHandle materialHandle = assets.CreateMaterial(materialDesc);

                std::string primitiveName = importedMesh.name.empty() ? "Primitive " + std::to_string(primIndex) : importedMesh.name + " Primitive " + std::to_string(primIndex);
                Entity primitiveEntity = scene.CreateEntity(primitiveName);

                scene.SetParent(primitiveEntity, entity);
                TransformComponent& primitiveTransform = scene.AddTransform(primitiveEntity);
                primitiveTransform.position = {};
                primitiveTransform.rotation = Math::Quat::Identity();
                primitiveTransform.scale = { 1.0f, 1.0f, 1.0f };
                MeshRendererComponent& renderer = scene.AddMeshRenderer(primitiveEntity);
                renderer.mesh = meshHandle;
                renderer.material = materialHandle;

                primIndex++;
            }
        }

        for (int childNodeIndex : importedNode.children)
        {
            ImportNode(scene, assets, model, childNodeIndex, entity);
        }

        return entity;
    }
}