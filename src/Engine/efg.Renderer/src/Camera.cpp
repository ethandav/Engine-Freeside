#include "..\include\Camera.h"

namespace efg
{
    void Camera::SetPosition(const Vec3& position)
    {
        m_position = position;
    }

    void Camera::SetTarget(const Vec3& target)
    {
        m_target = target;
    }

    void Camera::SetUp(const Vec3& up)
    {
        m_up = up;
    }

    CameraConstants Camera::BuildCameraConstants() const
    {
        CameraConstants constants = {};
        constants.viewProjection = Transpose(GetViewProjectionMatrix());
        Vec3 camPosition = GetPosition();
        constants.viewPosition = Vec4(camPosition.x, camPosition.y, camPosition.z, 0.0f);
        return constants;
    }

    void Camera::LookAt(const Vec3& position, const Vec3& target, const Vec3& up)
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

    const Vec3& Camera::GetPosition() const
    {
        return m_position;
    }

    const Vec3& Camera::GetTarget() const
    {
        return m_target;
    }

    const Vec3& Camera::GetUp() const
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

    Mat4 Camera::GetViewMatrix() const
    {
        return LookAtLH(m_position, m_target, m_up);
    }

    Mat4 Camera::GetProjectionMatrix() const
    {
        return PerspectiveFovLH(
            m_fovYRadians,
            m_aspectRatio,
            m_nearZ,
            m_farZ);
    }

    Mat4 Camera::GetViewProjectionMatrix() const
    {
        Mat4 view = GetViewMatrix();
        Mat4 projection = GetProjectionMatrix();

        return projection * view;
    }
}