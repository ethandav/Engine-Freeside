#pragma once

#include "Scene.h"
#include "..\..\Freeside.Assets\include\ImportedAssetTypes.h"
#include "..\..\Freeside.Assets\include\AssetManager.h"

namespace Freeside
{
    namespace Scene
    {
        class SceneImporter
        {
        public:
            static Entity ImportModel(Scene& scene, Assets::AssetManager& assets, const Assets::ImportedModel& model);

        private:
            static Entity ImportNode(Scene& scene, Assets::AssetManager& assets, const Assets::ImportedModel& model, int nodeIndex, Entity parent);
        };
    }
}