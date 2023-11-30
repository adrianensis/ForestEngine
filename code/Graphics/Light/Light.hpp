#pragma once

#include "Scene/Component.hpp"

class LightData
{
public:
    Vector3 mPosition;
};

class Light: public Component
{
    GENERATE_METADATA(Light)

public:
    void init(const LightData& data);

private:
    LightData mLightData;

public:
    CRGET(LightData)
};