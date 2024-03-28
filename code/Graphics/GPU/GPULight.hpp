#pragma once

#include "Engine/Minimal.hpp"

class GPULight
{
public:
    alignas(16) Vector3 mPosition = Vector3::smZero;
    alignas(16) Vector3 mAmbient = Vector3::smOne;
    alignas(16) Vector3 mDiffuse = Vector3::smOne;
    alignas(16) Vector3 mSpecular = Vector3::smOne;
    f32 mAmbientIntensity = 1.0f;
    f32 mDiffuseIntensity = 1.0f;
    f32 mSpecularIntensity = 1.0f;
};

class GPULightsData
{
public:
    GPULight mLights[10];
};