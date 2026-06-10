#include "..\..\include\controllers\FirstPersonCameraController.h"
#include <algorithm>

namespace Freeside
{
    Math::Vec3 FirstPersonCameraController::GetForward(const FirstPersonCameraControllerComponent& controller)
    {
        const float cosPitch = std::cos(controller.pitch);

        return Math::Normalize(Math::Vec3(
            std::sin(controller.yaw) * cosPitch,
            std::sin(controller.pitch),
            std::cos(controller.yaw) * cosPitch
        ));
    }

    Math::Vec3 FirstPersonCameraController::GetRight(const FirstPersonCameraControllerComponent& controller)
    {
        Math::Vec3 forward = GetForward(controller);

        forward.y = 0.0f;
        forward = Math::Normalize(forward);

        Math::Vec3 worldUp(0.0f, 1.0f, 0.0f);

        return Math::Normalize(Math::Cross(worldUp, forward));
    }

    void FirstPersonCameraController::Update(TransformComponent& transform, FirstPersonCameraControllerComponent& controller, const InputState& input, float deltaTime)
    {
        controller.yaw += input.mouseDeltaX * controller.lookSensitivity;
        controller.pitch -= input.mouseDeltaY * controller.lookSensitivity;

        constexpr float MaxPitch = 1.55334306f;

        controller.pitch = std::clamp(
            controller.pitch,
            -MaxPitch,
            MaxPitch
        );

        Math::Vec3 forward = GetForward(controller);
        Math::Vec3 moveForward = forward;
        moveForward.y = 0.0f;

        if (Math::LengthSquared(moveForward) > 0.0f)
        {
            moveForward = Math::Normalize(moveForward);
        }

        Math::Vec3 right = GetRight(controller);

        Math::Vec3 moveDirection(0.0f, 0.0f, 0.0f);

        if (input.IsKeyDown(Key::W))
            moveDirection += moveForward;

        if (input.IsKeyDown(Key::S))
            moveDirection -= moveForward;

        if (input.IsKeyDown(Key::D))
            moveDirection += right;

        if (input.IsKeyDown(Key::A))
            moveDirection -= right;

        if (controller.allowVerticalMovement)
        {
            if (input.IsKeyDown(Key::Space))
                moveDirection.y += 1.0f;

            if (input.IsKeyDown(Key::LeftCtrl))
                moveDirection.y -= 1.0f;
        }

        if (Math::LengthSquared(moveDirection) > 0.0f)
        {
            moveDirection = Math::Normalize(moveDirection);
        }

        transform.position += moveDirection * controller.moveSpeed * deltaTime;

        transform.rotation = Math::FromEulerXYZ(
            -controller.pitch,
            controller.yaw,
            0.0f
        );
    }
}