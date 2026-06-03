#include "..\..\include\controllers\FirstPersonCameraController.h"

namespace Freeside
{
    void FirstPersonCameraController::InitializeFromCamera(const Camera& camera)
    {
        Math::Vec3 direction = Math::Normalize(camera.GetTarget() - camera.GetPosition());

        m_pitch = std::asin(direction.y);

        m_yaw = std::atan2(direction.x, direction.z);
    }

    Math::Vec3 FirstPersonCameraController::GetForward() const
    {
        const float cosPitch = std::cos(m_pitch);

        return Math::Normalize(Math::Vec3(
            std::sin(m_yaw) * cosPitch,
            std::sin(m_pitch),
            std::cos(m_yaw) * cosPitch
        ));
    }

    Math::Vec3 FirstPersonCameraController::GetRight() const
    {
        Math::Vec3 forward = GetForward();

        // Remove pitch for ground-plane right vector.
        forward.y = 0.0f;
        forward = Math::Normalize(forward);

        Math::Vec3 worldUp(0.0f, 1.0f, 0.0f);

        return Math::Normalize(Math::Cross(worldUp, forward));
    }

    void FirstPersonCameraController::Update(Camera* camera, const InputState& input, float deltaTime)
    {
        // Mouse look.
        m_yaw += input.mouseDeltaX * m_mouseSensitivity;
        m_pitch -= input.mouseDeltaY * m_mouseSensitivity;

        // Clamp pitch to avoid flipping at straight up/down.
        constexpr float MaxPitch = 1.55334306f; // ~89 degrees
        if (m_pitch > MaxPitch)
            m_pitch = MaxPitch;
        if (m_pitch < -MaxPitch)
            m_pitch = -MaxPitch;

        Freeside::Math::Vec3 forward = GetForward();

        // Movement should usually be horizontal for FPS-style walking.
        Freeside::Math::Vec3 moveForward = forward;
        moveForward.y = 0.0f;

        if (Freeside::Math::LengthSquared(moveForward) > 0.0f)
        {
            moveForward = Freeside::Math::Normalize(moveForward);
        }

        Freeside::Math::Vec3 right = GetRight();

        Freeside::Math::Vec3 moveDirection(0.0f, 0.0f, 0.0f);

        if (input.IsKeyDown(Key::W))
            moveDirection += moveForward;

        if (input.IsKeyDown(Key::S))
            moveDirection -= moveForward;

        if (input.IsKeyDown(Key::D))
            moveDirection += right;

        if (input.IsKeyDown(Key::A))
            moveDirection -= right;

        if (input.IsKeyDown(Key::Space))
            moveDirection.y += 1.0f;

        if (input.IsKeyDown(Key::LeftCtrl))
            moveDirection.y -= 1.0f;

        if (Freeside::Math::LengthSquared(moveDirection) > 0.0f)
        {
            moveDirection = Freeside::Math::Normalize(moveDirection);
        }

        Freeside::Math::Vec3 position = camera->GetPosition();
        position += moveDirection * m_moveSpeed * deltaTime;

        camera->SetPosition(position);
        camera->SetTarget(position + forward);
        camera->SetUp(Freeside::Math::Vec3(0.0f, 1.0f, 0.0f));
    }
}