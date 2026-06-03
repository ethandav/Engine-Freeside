#pragma once

#include "..\..\..\Freeside.Core\include\math\math.h"

namespace Freeside
{
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

    class Camera
    {
    public:
        Camera() = default;

        void SetPosition(const Freeside::Math::Vec3& position);
        void SetTarget(const Freeside::Math::Vec3& target);
        void SetUp(const Freeside::Math::Vec3& up);

        void LookAt(const Freeside::Math::Vec3& position, const Freeside::Math::Vec3& target, const Freeside::Math::Vec3& up = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f));

        void SetPerspective(float fovYRadians, float aspectRatio, float nearZ, float farZ);
        void SetOrthographic(float width, float height, float nearZ, float farZ);

        const Freeside::Math::Vec3& GetPosition() const;
        const Freeside::Math::Vec3& GetTarget() const;
        const Freeside::Math::Vec3& GetUp() const;

        float GetFovYRadians() const;
        float GetAspectRatio() const;
        float GetNearZ() const;
        float GetFarZ() const;

        Freeside::Math::Mat4 GetViewMatrix() const;
        Freeside::Math::Mat4 GetProjectionMatrix() const;
        Freeside::Math::Mat4 GetViewProjectionMatrix() const;

    private:
        Freeside::Math::Vec3 m_position = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
        Freeside::Math::Vec3 m_target = Freeside::Math::Vec3(0.0f, 0.0f, 0.0f);
        Freeside::Math::Vec3 m_up = Freeside::Math::Vec3(0.0f, 1.0f, 0.0f);

        float m_fovYRadians = 0.78539816339f; // 45 degrees
        float m_aspectRatio = 16.0f / 9.0f;
        float m_nearZ = 0.1f;
        float m_farZ = 1000.0f;

        ProjectionType m_projectionType = ProjectionType::Perspective;

        float m_orthoWidth = 10.0f;
        float m_orthoHeight = 10.0f;
    };
}