#include "Graphics/Light/Light.hpp"

ClassId Light::getSystemComponentId() const { return ClassManager::getClassMetadata<Light>().mClassDefinition.mId; }

void PointLight::init(const PointLightData& data)
{
    mLightData = data;
}

void DirectionalLight::init(const DirectionalLightData& data)
{
    mLightData = data;
}