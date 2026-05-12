#pragma once

#include "RenderTypes.h"
#include "MeshData.h"
#include "Materials.h"

namespace efg
{
    struct DrawBatch
    {
        MeshHandle mesh;
        MaterialHandle material;

        uint32_t firstSortedIndex = 0;
        uint32_t instanceCount = 0;
    };

    class RenderQueue
    {
    public:
        std::vector<uint32_t> sortedIndices;
        std::vector<DrawBatch> batches;

        void Clear();
        void BuildForwardGeometryBatches(const std::vector<RenderObject>& objects);
        void SortRenderObjectsForInstancing(const std::vector<RenderObject>& objects);
    };
}