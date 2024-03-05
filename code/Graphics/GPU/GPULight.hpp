#pragma once

#include "Engine/Minimal.hpp"

class GPULight
{
public:
    alignas(16) Vector3 mPosition = Vector3::smZero;
    alignas(16) Vector3 mColor = Vector3::smOne;
    f32 mAmbientIntensity = 0.2f;
    f32 mSpecularIntensity = 0.5f;
};

class GPULightsData
{
public:
    GPULight mLights[10];
};