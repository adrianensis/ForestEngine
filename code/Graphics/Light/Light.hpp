#pragma once

#include "Scene/Component.hpp"
#include "Graphics/GPU/GPULight.hpp"

class Light: public Component
{
public:
    ClassId getSystemComponentId() const override;
    virtual Matrix4 getLightProjectionViewMatrix() const;
};
REGISTER_CLASS(Light);

class PointLight: public Light
{
public:
    void init(const PointLightData& data);
    PointLightData calculateLightData() const;

private:
    PointLightData mLightData;

public:
    CRGET(LightData)
};
REGISTER_CLASS(PointLight);

class DirectionalLight: public Light
{
public:
    void init(const DirectionalLightData& data);
    DirectionalLightData calculateLightData() const;

private:
    DirectionalLightData mLightData;

public:
    CRGET(LightData)
};
REGISTER_CLASS(DirectionalLight);