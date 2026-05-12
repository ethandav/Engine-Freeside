#include "..\..\include\render\RenderQueue.h"
#include <algorithm>

namespace efg
{
	void RenderQueue::Clear()
	{
        batches.clear();
	}

	void RenderQueue::BuildForwardGeometryBatches(const std::vector<RenderObject>& objects)
	{
        Clear();
        sortedIndices.resize(objects.size());
        SortRenderObjectsForInstancing(objects);
        uint32_t begin = 0;

        while (begin < sortedIndices.size())
        {
            const RenderObject& first = objects[sortedIndices[begin]];

            uint32_t end = begin + 1;

            while (end < sortedIndices.size())
            {
                const RenderObject& candidate = objects[sortedIndices[end]];

                if (candidate.mesh != first.mesh ||
                    candidate.material != first.material)
                {
                    break;
                }

                ++end;
            }

            batches.push_back({first.mesh, first.material, begin, (end - begin)});

            begin = end;
        }
	}

    void RenderQueue::SortRenderObjectsForInstancing(const std::vector<RenderObject>& objects)
    {
        for (uint32_t i = 0; i < sortedIndices.size(); ++i)
        {
            sortedIndices[i] = i;
        }

        std::sort(sortedIndices.begin(), sortedIndices.end(),
            [&](uint32_t lhs, uint32_t rhs)
            {
                const RenderObject& a = objects[lhs];
                const RenderObject& b = objects[rhs];

                if (a.material.index != b.material.index)
                    return a.material.index < b.material.index;

                return a.mesh.index < b.mesh.index;
            });
    }
}