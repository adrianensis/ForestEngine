#pragma once

#include "Scene/Component.hpp"
#include "Graphics/GPU/GPULight.hpp"

class Light: public Component
{
public:
    ClassId getSystemComponentId() const override;
    void init(const GPULight& data);

private:
    GPULight mLightData;

public:
    CRGET(LightData)
};
REGISTER_CLASS(Light);
