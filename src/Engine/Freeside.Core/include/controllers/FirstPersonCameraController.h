#pragma once

#include "..\..\..\Freeside.Platform\include\InputState.h"
#include "..\..\..\Freeside.World\include\Entity.h"

namespace Freeside
{
    struct FirstPersonCameraController
    {
        static void Update(TransformComponent& transform, FirstPersonCameraControllerComponent& controller, const InputState& input, float deltaTime);
    private:
        static Math::Vec3 GetForward(const FirstPersonCameraControllerComponent& controller);
        static Math::Vec3 GetRight(const FirstPersonCameraControllerComponent& controller);
    };
}