#pragma once

#include "Scene/Component.hpp"
#include "Graphics/GPU/GPULight.hpp"

class Light: public Component
{
    GENERATE_METADATA(Light)

public:
    ClassId getSystemComponentId() const override { return getClassDefinitionStatic().mId; }
    void init(const GPULight& data);

private:
    GPULight mLightData;

public:
    CRGET(LightData)
};