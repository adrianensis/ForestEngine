#pragma once

#include "Core/Module.hpp"

class GPULightData
{
public:
    Vector3 mPosition = Vector3(0,0,0);
    Vector4 mColor = Vector4(1,1,1,1);
    f32 mStrength = 0.1f;
};