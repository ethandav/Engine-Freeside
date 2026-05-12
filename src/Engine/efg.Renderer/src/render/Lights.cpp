#include "..\..\include\render\lights.h"

namespace efg
{
    namespace Lights
    {
        DirectionalLightConstants Directional::BuildDirectionalLightConstants() const
        {
            DirectionalLightConstants constants = {};
            constants.directionAndIntensity = {
                direction.x,
                direction.y,
                direction.z,
                1.0f
            };
            constants.colorAndPadding = {
                color.x,
                color.y,
                color.z,
                1.0f
            };
            return constants;
        }

        void Point::BuildGpuPointLights()
        {
        }
    }
}