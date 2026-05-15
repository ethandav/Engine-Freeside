#pragma once

#include "types/Handles.h"
#include "types/RenderObject.h"

#include <vector>

namespace efg
{
    struct DrawBatch
    {
        Freeside::MeshHandle mesh;
        Freeside::MaterialHandle material;

        uint32_t firstSortedIndex = 0;
        uint32_t instanceCount = 0;
    };

    class RenderQueue
    {
    public:
        std::vector<uint32_t> sortedIndices;
        std::vector<DrawBatch> batches;

        void Clear();
        void BuildForwardGeometryBatches(const std::vector<Freeside::RenderObject>& objects);
        void SortRenderObjectsForInstancing(const std::vector<Freeside::RenderObject>& objects);
    };
}