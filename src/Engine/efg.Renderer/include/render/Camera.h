#pragma once

#include "..\..\..\efg.Core\include\math\math.h"

namespace efg
{
    struct CameraConstants
    {
        Math::Vec4 viewPosition;
        Math::Mat4 viewProjection;
    };

    class Camera
    {
    public:
        Camera() = default;

        void SetPosition(const Math::Vec3& position);
        void SetTarget(const Math::Vec3& target);
        void SetUp(const Math::Vec3& up);
        CameraConstants BuildCameraConstants() const;

        void LookAt(const Math::Vec3& position, const Math::Vec3& target, const Math::Vec3& up = Math::Vec3(0.0f, 1.0f, 0.0f));

        void SetPerspective(
            float fovYRadians,
            float aspectRatio,
            float nearZ,
            float farZ);

        const Math::Vec3& GetPosition() const;
        const Math::Vec3& GetTarget() const;
        const Math::Vec3& GetUp() const;

        float GetFovYRadians() const;
        float GetAspectRatio() const;
        float GetNearZ() const;
        float GetFarZ() const;

        Math::Mat4 GetViewMatrix() const;
        Math::Mat4 GetProjectionMatrix() const;
        Math::Mat4 GetViewProjectionMatrix() const;

    private:
        Math::Vec3 m_position = Math::Vec3(0.0f, 0.0f, -5.0f);
        Math::Vec3 m_target = Math::Vec3(0.0f, 0.0f, 0.0f);
        Math::Vec3 m_up = Math::Vec3(0.0f, 1.0f, 0.0f);

        float m_fovYRadians = 0.78539816339f; // 45 degrees
        float m_aspectRatio = 16.0f / 9.0f;
        float m_nearZ = 0.1f;
        float m_farZ = 1000.0f;
    };
}