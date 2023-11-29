#pragma once

#include "Scene/Component.hpp"

class LightData: public ComponentData
{
public:
    Vector3 mPosition;
};

class Light: public ComponentWithData<LightData>
{
    GENERATE_METADATA(Light)

public:
    void init() {};
};