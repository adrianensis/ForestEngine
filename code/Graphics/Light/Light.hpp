#pragma once

#include "Scene/Component.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class LightData
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

class LightBuiltIn
{
public:

    inline static const GPUStructDefinition mLightStructDefinition
    {
        "light",
        {
            {GPUBuiltIn::PrimitiveTypes::mVector3, "position"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "ambient"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "diffuse"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "specular"},
            {GPUBuiltIn::PrimitiveTypes::mFloat, "ambientIntensity"},
            {GPUBuiltIn::PrimitiveTypes::mFloat, "diffuseIntensity"},
            {GPUBuiltIn::PrimitiveTypes::mFloat, "specularIntensity"}
        }
    };

    inline static const GPUDataType mLightStructDataType{mLightStructDefinition.mName, mLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};

    inline static const GPUSharedBufferData mLightsBufferData
    {
        GPUBufferType::UNIFORM,
        {
            {{GPUStorage::UNIFORM, mLightStructDataType, "lights"}, "", std::to_string(10)},
            // {GPUStorage::UNIFORM, PrimitiveTypes::mFloat, "ambientIntensity"},
        },
        "LightsData",
        "lightsData"
    };

    class LightsData
    {
    public:
        LightData mLights[10];
    };
};

class Light: public Component
{
public:
    ClassId getSystemComponentId() const override;
    void init(const LightData& data);

private:
    LightData mLightData;

public:
    CRGET(LightData)
};
REGISTER_CLASS(Light);