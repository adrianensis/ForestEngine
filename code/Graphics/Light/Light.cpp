#include "Graphics/Light/Light.hpp"

ClassId Light::getSystemComponentId() const { return ClassManager::getClassMetadata<Light>().mClassDefinition.mId; }

void Light::init(const LightData& data)
{
    mLightData = data;
}