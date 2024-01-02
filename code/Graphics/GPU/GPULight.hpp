#pragma once

#include "Core/Module.hpp"

class GPULight
{
public:
    alignas(16) Vector3 mPosition = Vector3(0,0,0);
    alignas(16) Vector4 mColor = Vector4(1,1,1,1);
    alignas(16) f32 mStrength = 0.2f;
};

class GPULightsData
{
public:
    alignas(16) GPULight mLights[10];
    alignas(16) f32 mAmbient = 0.2f;
};