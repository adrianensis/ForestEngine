#pragma once

#include "GPU/Shader/GPUShaderDefinitions.hpp"

class AmbientLightData
{
public:
    Maths::Vector4 mDiffuse = Maths::Vector4::smZero;
};

class DirectionalLightData
{
public:
    Maths::Vector4 mDirection = Maths::Vector4::smZero;
    Maths::Vector4 mDiffuse = Maths::Vector4::smZero;
};

class PointLightData
{
public:
    Maths::Vector4 mPosition = Maths::Vector4::smZero;
    Maths::Vector4 mDiffuse = Maths::Vector4::smZero;
};

class SpotLightData
{
public:
    Maths::Vector4 mPosition = Maths::Vector4::smZero;
    Maths::Vector4 mDirection = Maths::Vector4::smZero;
    Maths::Vector4 mDiffuse = Maths::Vector4::smZero;
    alignas(16) Core::f32 mInnerCutOff = 0;
    Core::f32 mOuterCutOff = 0;
};

class GPULightBuiltIn
{
public:

    inline static const GPUStructDefinition mAmbientLightStructDefinition
    {
        "ambientLight",
        {
            {GPUShaderDefinitions::PrimitiveTypes::mVector4, "diffuse"}
        }
    };

    inline static const GPUStructDefinition mDirectionalLightStructDefinition
    {
        "directionalLight",
        {
            {GPUShaderDefinitions::PrimitiveTypes::mVector4, "direction"},
            {GPUShaderDefinitions::PrimitiveTypes::mVector4, "diffuse"}
        }
    };

    inline static const GPUStructDefinition mPointLightStructDefinition
    {
        "pointLight",
        {
            {GPUShaderDefinitions::PrimitiveTypes::mVector4, "position"},
            {GPUShaderDefinitions::PrimitiveTypes::mVector4, "diffuse"}
        }
    };

    inline static const GPUStructDefinition mSpotLightStructDefinition
    {
        "spotLight",
        {
            {GPUShaderDefinitions::PrimitiveTypes::mVector4, "position"},
            {GPUShaderDefinitions::PrimitiveTypes::mVector4, "direction"},
            {GPUShaderDefinitions::PrimitiveTypes::mVector4, "diffuse"},
            {GPUShaderDefinitions::PrimitiveTypes::mFloat, "innerCutOff"},
            {GPUShaderDefinitions::PrimitiveTypes::mFloat, "outerCutOff"}
        }
    };

    inline static const GPUDataType mPointLightStructDataType{mPointLightStructDefinition.mName, mPointLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    inline static const GPUDataType mSpotLightStructDataType{mSpotLightStructDefinition.mName, mSpotLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    inline static const GPUDataType mDirectionalLightStructDataType{mDirectionalLightStructDefinition.mName, mDirectionalLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    inline static const GPUDataType mAmbientLightStructDataType{mAmbientLightStructDefinition.mName, mAmbientLightStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};

    inline static const GPUUniformBufferData mLightsBufferData
    {
        GPUBufferType::UNIFORM,
        {
            {{GPUStorage::UNIFORM, mDirectionalLightStructDataType, "directional"}},
            {{GPUStorage::UNIFORM, mAmbientLightStructDataType, "ambient"}},
            {{GPUStorage::UNIFORM, mPointLightStructDataType, "pointLights"}, "", std::to_string(5)},
            {{GPUStorage::UNIFORM, mSpotLightStructDataType, "spotLights"}, "", std::to_string(5)},
            // {GPUStorage::UNIFORM, PrimitiveTypes::mFloat, "ambientIntensity"},
        },
        "LightsData",
        "lightsData",
        GPUDescriptorSetScope::LOCAL
    };

    class LightsData
    {
    public:
        DirectionalLightData mDirectionalLight;
        AmbientLightData mAmbientLight;
        PointLightData mPointLights[5];
        SpotLightData mSpotLights[5];
    };

    inline static const GPUUniformBufferData mShadowMappingBufferData
    {
        GPUBufferType::UNIFORM,
        {
            {{GPUStorage::UNIFORM, GPUShaderDefinitions::PrimitiveTypes::mMatrix4, "lightProjectionViewMatrix"}}
        },
        "ShadowMapping",
        "shadowMapping",
        GPUDescriptorSetScope::LOCAL
    };

    class ShadowMappingData
    {
    public:
        Maths::Matrix4 mLightProjectionViewMatrix;
    };
};