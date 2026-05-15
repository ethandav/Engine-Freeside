#include "..\..\include\render\Camera.h"

namespace Freeside
{
    void Camera::SetPosition(const Math::Vec3& position)
    {
        m_position = position;
    }

    void Camera::SetTarget(const Math::Vec3& target)
    {
        m_target = target;
    }

    void Camera::SetUp(const Math::Vec3& up)
    {
        m_up = up;
    }

    CameraConstants Camera::BuildCameraConstants() const
    {
        CameraConstants constants = {};
        constants.viewProjection = Transpose(GetViewProjectionMatrix());
        Math::Vec3 camPosition = GetPosition();
        constants.viewPosition = Math::Vec4(camPosition.x, camPosition.y, camPosition.z, 0.0f);
        return constants;
    }

    void Camera::LookAt(const Math::Vec3& position, const Math::Vec3& target, const Math::Vec3& up)
    {
        m_position = position;
        m_target = target;
        m_up = up;
    }

    void Camera::SetPerspective(
        float fovYRadians,
        float aspectRatio,
        float nearZ,
        float farZ)
    {
        m_fovYRadians = fovYRadians;
        m_aspectRatio = aspectRatio;
        m_nearZ = nearZ;
        m_farZ = farZ;
    }

    const Math::Vec3& Camera::GetPosition() const
    {
        return m_position;
    }

    const Math::Vec3& Camera::GetTarget() const
    {
        return m_target;
    }

    const Math::Vec3& Camera::GetUp() const
    {
        return m_up;
    }

    float Camera::GetFovYRadians() const
    {
        return m_fovYRadians;
    }

    float Camera::GetAspectRatio() const
    {
        return m_aspectRatio;
    }

    float Camera::GetNearZ() const
    {
        return m_nearZ;
    }

    float Camera::GetFarZ() const
    {
        return m_farZ;
    }

    Math::Mat4 Camera::GetViewMatrix() const
    {
        return LookAtLH(m_position, m_target, m_up);
    }

    Math::Mat4 Camera::GetProjectionMatrix() const
    {
        return Math::PerspectiveFovLH(
            m_fovYRadians,
            m_aspectRatio,
            m_nearZ,
            m_farZ);
    }

    Math::Mat4 Camera::GetViewProjectionMatrix() const
    {
        Math::Mat4 view = GetViewMatrix();
        Math::Mat4 projection = GetProjectionMatrix();

        return projection * view;
    }
}