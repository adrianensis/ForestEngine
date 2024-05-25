#pragma once

#include "Graphics/Material/Shader/ShaderDefault.hpp"

// PBR METALLIC

class TextureBindingNamesPBR : public TextureBindingNames
{
public:
    inline static const ConstString smNormal = "Normal";
    inline static const ConstString smMetallicRoughness = "MetallicRoughness";
    inline static const ConstString smShadowMap = "ShadowMap";
};

class MetallicRoughness
{
public:
    Vector4 mBaseColor = Vector4(0,0,0,1);
    alignas(16) f32 mMetallic = 0;
    alignas(16) f32 mRoughness = 0;
};

class ShaderPBR : public ShaderDefault
{
protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock() override;

    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const override;

    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;

    void registerFunctionsShadowCalculation(ShaderBuilder& shaderBuilder) const;
    void registerFunctionsPBRHelpers(ShaderBuilder& shaderBuilder) const;
    void registerFunctionCalculatePBR(ShaderBuilder& shaderBuilder) const;

protected:
    inline static const GPUFunctionDefinition mCalculateShadow { GPUBuiltIn::PrimitiveTypes::mFloat, "calculateShadow",
        {
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector4, "fragPosLightSpace"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "lightDirection"}
        }};

    inline static const GPUFunctionDefinition mCalculatePBR { GPUBuiltIn::PrimitiveTypes::mVector4, "calculatePBR",
        {
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "baseColor"}
        }};
    inline static const GPUFunctionDefinition mCalculatePBRSingleLight { GPUBuiltIn::PrimitiveTypes::mVector3, "calculatePBRSingleLight",
        {
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "albedo"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mFloat, "metallic"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mFloat, "roughness"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "V"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "N"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "F0"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "lightDirection"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "lightColor"}
        }};
    inline static const GPUFunctionDefinition mGetNormalFromMap { GPUBuiltIn::PrimitiveTypes::mVector3, "getNormalFromMap" };
    inline static const GPUFunctionDefinition mDistributionGGX { GPUBuiltIn::PrimitiveTypes::mFloat, "distributionGGX",
        {
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "N"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "H"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mFloat, "roughness"}
        }};
    inline static const GPUFunctionDefinition mGeometrySchlickGGX { GPUBuiltIn::PrimitiveTypes::mFloat, "geometrySchlickGGX",
        {
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mFloat, "NdotV"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mFloat, "roughness"}
        }};
    inline static const GPUFunctionDefinition mGeometrySmith { GPUBuiltIn::PrimitiveTypes::mFloat, "geometrySmith",
        {
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "N"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "V"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "L"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mFloat, "roughness"}
        }};
    inline static const GPUFunctionDefinition mFresnelSchlick { GPUBuiltIn::PrimitiveTypes::mVector3, "fresnelSchlick",
        {
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mFloat, "cosTheta"},
            {GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector3, "F0"}
        }};
};
