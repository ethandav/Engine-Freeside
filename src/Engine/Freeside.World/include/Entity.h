#include <cstdint>
#include "..\..\Freeside.Core\include\math\Vec3.h"
#include "..\..\Freeside.Core\include\math\Mat4.h"
#include "..\..\efg\include\render\types\Handles.h"
#include "..\..\efg\include\render\Camera.h"

namespace Freeside
{
    using EntityId = uint32_t;

    struct Entity
    {
        EntityId id = 0;
    };

    struct TransformComponent
    {
        Math::Vec3 position = {};
        Math::Vec3 rotation = {};
        Math::Vec3 scale = { 1.0f, 1.0f, 1.0f };

        Math::Mat4 GetWorldMatrix() const { return Freeside::Math::TransformMatrix(position, rotation, scale); };
    };

    struct MeshRendererComponent
    {
        MeshHandle mesh = {};
        MaterialHandle material = {};
    };

    struct CameraComponent
    {
        Camera camera = {};
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
}