#pragma once

#include "GPU/Shader/GPUShaderDefinitions.hpp"

class DirectionalLightData
{
public:
    alignas(16) Vector3 mDirection = Vector3::smZero;
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
    alignas(16) Vector3 mDirection = Vector3::smZero;
    alignas(16) Vector3 mDiffuse = Vector3::smZero;
    alignas(16) Core::f32 mInnerCutOff = 0;
    Core::f32 mOuterCutOff = 0;
};

class GPULightBuiltIn
{
public:

    inline static const GPUStructDefinition mDirectionalLightStructDefinition
    {
        "directionalLight",
        {
            {GPUShaderDefinitions::PrimitiveTypes::mVector3, "direction"},
            {GPUShaderDefinitions::PrimitiveTypes::mVector3, "diffuse"}
        }
    };

    inline static const GPUStructDefinition mPointLightStructDefinition
    {
        "pointLight",
        {
            {GPUShaderDefinitions::PrimitiveTypes::mVector3, "position"},
            {GPUShaderDefinitions::PrimitiveTypes::mVector3, "diffuse"}
        }
    };

    inline static const GPUStructDefinition mSpotLightStructDefinition
    {
        "spotLight",
        {
            {GPUShaderDefinitions::PrimitiveTypes::mVector3, "position"},
            {GPUShaderDefinitions::PrimitiveTypes::mVector3, "direction"},
            {GPUShaderDefinitions::PrimitiveTypes::mVector3, "diffuse"},
            {GPUShaderDefinitions::PrimitiveTypes::mFloat, "innerCutOff"},
            {GPUShaderDefinitions::PrimitiveTypes::mFloat, "outerCutOff"}
        }
    };

    inline static const GPUDataType mPointLightStructDataType{mPointLightStructDefinition.mName, mPointLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    inline static const GPUDataType mSpotLightStructDataType{mSpotLightStructDefinition.mName, mSpotLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    inline static const GPUDataType mDirectionalLightStructDataType{mDirectionalLightStructDefinition.mName, mDirectionalLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};

    inline static const GPUUniformBufferData mLightsBufferData
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

    inline static const GPUUniformBufferData mShadowMappingBufferData
    {
        GPUBufferType::UNIFORM,
        {
            {{GPUStorage::UNIFORM, GPUShaderDefinitions::PrimitiveTypes::mMatrix4, "lightProjectionViewMatrix"}}
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