#pragma once

#include "Scene/Component.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"


class DirectionalLightData
{
public:
    alignas(16) Vector3 mDirection = Vector3::smOne;
    alignas(16) Vector3 mDiffuse = Vector3::smZero;
};

class PointLightData
{
public:
    alignas(16) Vector3 mPosition = Vector3::smZero;
    alignas(16) Vector3 mDiffuse = Vector3::smZero;
};

class SpotLightData
{
public:
    alignas(16) Vector3 mPosition = Vector3::smZero;
    alignas(16) Vector3 mDirection = Vector3::smOne;
    alignas(16) Vector3 mDiffuse = Vector3::smZero;
    alignas(16) f32 mInnerCutOff = 0;
    f32 mOuterCutOff = 0;
};

class LightBuiltIn
{
public:

    inline static const GPUStructDefinition mDirectionalLightStructDefinition
    {
        "directionalLight",
        {
            {GPUBuiltIn::PrimitiveTypes::mVector3, "direction"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "diffuse"}
        }
    };

    inline static const GPUStructDefinition mPointLightStructDefinition
    {
        "pointLight",
        {
            {GPUBuiltIn::PrimitiveTypes::mVector3, "position"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "diffuse"}
        }
    };

    inline static const GPUStructDefinition mSpotLightStructDefinition
    {
        "spotLight",
        {
            {GPUBuiltIn::PrimitiveTypes::mVector3, "position"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "direction"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "diffuse"},
            {GPUBuiltIn::PrimitiveTypes::mFloat, "innerCutOff"},
            {GPUBuiltIn::PrimitiveTypes::mFloat, "outerCutOff"}
        }
    };

    inline static const GPUDataType mPointLightStructDataType{mPointLightStructDefinition.mName, mPointLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    inline static const GPUDataType mSpotLightStructDataType{mSpotLightStructDefinition.mName, mSpotLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    inline static const GPUDataType mDirectionalLightStructDataType{mDirectionalLightStructDefinition.mName, mDirectionalLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};

    inline static const GPUSharedBufferData mLightsBufferData
    {
        GPUBufferType::UNIFORM,
        {
            {{GPUStorage::UNIFORM, mPointLightStructDataType, "pointLights"}, "", std::to_string(5)},
            {{GPUStorage::UNIFORM, mSpotLightStructDataType, "spotLights"}, "", std::to_string(5)},
            {{GPUStorage::UNIFORM, mDirectionalLightStructDataType, "directional"}},
            // {GPUStorage::UNIFORM, PrimitiveTypes::mFloat, "ambientIntensity"},
        },
        "LightsData",
        "lightsData"
    };

    class LightsData
    {
    public:
        PointLightData mPointLights[5];
        SpotLightData mSpotLights[5];
        DirectionalLightData mDirectionalLight;
    };

    inline static const GPUSharedBufferData mShadowMappingBufferData
    {
        GPUBufferType::UNIFORM,
        {
            {{GPUStorage::UNIFORM, GPUBuiltIn::PrimitiveTypes::mMatrix4, "lightProjectionViewMatrix"}}
        },
        "ShadowMapping",
        "shadowMapping"
    };

    class ShadowMappingData
    {
    public:
        Matrix4 mLightProjectionViewMatrix;
    };
};

class Light: public Component
{
public:
    ClassId getSystemComponentId() const override;
};
REGISTER_CLASS(Light);

class PointLight: public Light
{
public:
    void init(const PointLightData& data);

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

private:
    DirectionalLightData mLightData;

public:
    CRGET(LightData)
};
REGISTER_CLASS(DirectionalLight);