#pragma once

#include "..\..\..\efg\include\render\Camera.h"
#include "..\..\..\Freeside.Platform\include\InputState.h"

namespace Freeside
{
    class FirstPersonCameraController
    {
    public:
        void InitializeFromCamera(const Camera& camera);

        void Update(Camera* camera, const InputState& input, float deltaTime);

        void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
        void SetMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }

    private:
        Freeside::Math::Vec3 GetForward() const;
        Freeside::Math::Vec3 GetRight() const;

    private:
        float m_yaw = 0.0f;
        float m_pitch = 0.0f;

        float m_moveSpeed = 5.0f;
        float m_mouseSensitivity = 0.002f;
    };
}