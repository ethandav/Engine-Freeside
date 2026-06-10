#include <cstdint>
#include "..\..\Freeside.Core\include\math\Vec3.h"
#include "..\..\Freeside.Core\include\math\Mat4.h"
#include "..\..\Freeside.Core\include\math\Quat.h"
#include "..\..\efg\include\render\types\Handles.h"
#include "..\..\efg\include\render\Camera.h"

#include <vector>

namespace Freeside
{
    using EntityId = uint32_t;

    struct Entity
    {
        EntityId id = UINT32_MAX;

        Entity() = default;

        explicit Entity(EntityId idValue)
            : id(idValue)
        {
        }

        bool IsValid() const
        {
            return id != UINT32_MAX;
        }

        static Entity Invalid()
        {
            return Entity(UINT32_MAX);
        }
    };

    inline bool operator==(Entity a, Entity b)
    {
        return a.id == b.id;
    }

    inline bool operator!=(Entity a, Entity b)
    {
        return !(a == b);
    }

    struct TransformComponent
    {
        Math::Vec3 position = {};
        Math::Quat rotation = {};
        Math::Vec3 scale = { 1.0f, 1.0f, 1.0f };

        bool useMatrixOverride = false;
        Math::Mat4 matrixOverride = Math::Mat4::Identity();

        Math::Mat4 GetLocalMatrix() const
        {
            Math::Mat4 componentMatrix = Freeside::Math::TransformMatrix(position, rotation, scale);

            if (useMatrixOverride)
            {
                return componentMatrix * matrixOverride;
            }

            return componentMatrix;
        }
    };

    struct MeshRendererComponent
    {
        MeshHandle mesh = {};
        MaterialHandle material = {};
    };

    struct CameraComponent
    {
        ProjectionType projectionType = ProjectionType::Perspective;
        float fovYRadians = 0.78539816339f; // 45 degrees
        float nearZ = 0.1f;
        float farZ = 1000.0f;
        float orthographicWidth = 10.0f;
        float orthographicHeight = 10.0f;
        bool isMainCamera = false;
    };

    struct DirectionalLightComponent
    {
        Math::Vec3 direction = { 0.0f, -1.0f, 0.0f };
        Math::Vec3 color = { 1.0f, 1.0f, 1.0f };
        float intensity = 1.0f;
    };

    struct PointLightComponent
    {
        Math::Vec3 color = { 1.0f, 1.0f, 1.0f };
        float intensity = 1.0f;
        float radius = 5.0f;
    };

    struct SceneEnvironment
    {
        TextureHandle skyboxTexture = {};
    };

    struct HierarchyComponent
    {
        Entity parent = Entity(-1);
        std::vector<Entity> children;
    };
}