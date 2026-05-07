#pragma once

#include "..\..\efg.Core\include\math\math.h"
namespace efg
{
    struct CameraConstants
    {
        Vec4 viewPosition;
        Mat4 viewProjection;
    };

    class Camera
    {
    public:
        Camera() = default;

        void SetPosition(const Vec3& position);
        void SetTarget(const Vec3& target);
        void SetUp(const Vec3& up);

        void LookAt(const Vec3& position, const Vec3& target, const Vec3& up = Vec3(0.0f, 1.0f, 0.0f));

        void SetPerspective(
            float fovYRadians,
            float aspectRatio,
            float nearZ,
            float farZ);

        const Vec3& GetPosition() const;
        const Vec3& GetTarget() const;
        const Vec3& GetUp() const;

        float GetFovYRadians() const;
        float GetAspectRatio() const;
        float GetNearZ() const;
        float GetFarZ() const;

        Mat4 GetViewMatrix() const;
        Mat4 GetProjectionMatrix() const;
        Mat4 GetViewProjectionMatrix() const;

    private:
        Vec3 m_position = Vec3(0.0f, 0.0f, -5.0f);
        Vec3 m_target = Vec3(0.0f, 0.0f, 0.0f);
        Vec3 m_up = Vec3(0.0f, 1.0f, 0.0f);

        float m_fovYRadians = 0.78539816339f; // 45 degrees
        float m_aspectRatio = 16.0f / 9.0f;
        float m_nearZ = 0.1f;
        float m_farZ = 1000.0f;
    };
}