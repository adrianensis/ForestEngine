#pragma once

#include "Core/EntityComponent/Component.hpp"
#include "GPU/Light/GPULight.hpp"

class Light: public Component
{
public:
    Core::ClassId getComponentTypeId() const override;
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