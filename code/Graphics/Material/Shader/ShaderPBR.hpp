#pragma once

#include "Graphics/Material/Shader/ShaderDefault.hpp"

// PBR METALLIC

class TextureBindingNamesPBR : public TextureBindingNames
{
public:
    inline static const HashedString smNormal = "Normal";
    inline static const HashedString smMetallicRoughness = "MetallicRoughness";
    inline static const HashedString smShadowMap = "ShadowMap";
};

class PropertiesBlockShaderPBR
{
public:
    Vector4 mBaseColor = Vector4(0,0,0,1);
    alignas(16) f32 mMetallic = 0;
    alignas(16) f32 mRoughness = 0;
};
REGISTER_CLASS(PropertiesBlockShaderPBR)

class ShaderPBR : public ShaderDefault
{
public:
    virtual void generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const override;
protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock() override;
    virtual void registerTextures() override;
    virtual void vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const;
    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const override;

    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<GPUShaderDescriptorSets> gpuShaderDescriptorSets) const override;

    void registerFunctionsGetNormalFromMap(ShaderBuilder& shaderBuilder) const;
    void registerFunctionsShadowCalculation(ShaderBuilder& shaderBuilder) const;
    void registerFunctionsPBRHelpers(ShaderBuilder& shaderBuilder) const;
    void registerFunctionCalculatePBR(ShaderBuilder& shaderBuilder) const;

protected:
    inline static const GPUFunctionDefinition mCalculateShadow { GPUShaderDefinitions::PrimitiveTypes::mFloat, "calculateShadow",
        {
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector4, "fragPosLightSpace"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "lightDirection"}
        }};

    inline static const GPUFunctionDefinition mCalculatePBR { GPUShaderDefinitions::PrimitiveTypes::mVector4, "calculatePBR",
        {
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "baseColor"}
        }};
    inline static const GPUFunctionDefinition mCalculatePBRSingleLight { GPUShaderDefinitions::PrimitiveTypes::mVector3, "calculatePBRSingleLight",
        {
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "albedo"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mFloat, "metallic"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mFloat, "roughness"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "V"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "N"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "F0"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "lightDirection"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "lightColor"}
        }};
    inline static const GPUFunctionDefinition mGetNormalFromMap { GPUShaderDefinitions::PrimitiveTypes::mVector3, "getNormalFromMap" };
    inline static const GPUFunctionDefinition mDistributionGGX { GPUShaderDefinitions::PrimitiveTypes::mFloat, "distributionGGX",
        {
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "N"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "H"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mFloat, "roughness"}
        }};
    inline static const GPUFunctionDefinition mGeometrySchlickGGX { GPUShaderDefinitions::PrimitiveTypes::mFloat, "geometrySchlickGGX",
        {
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mFloat, "NdotV"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mFloat, "roughness"}
        }};
    inline static const GPUFunctionDefinition mGeometrySmith { GPUShaderDefinitions::PrimitiveTypes::mFloat, "geometrySmith",
        {
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "N"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "V"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "L"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mFloat, "roughness"}
        }};
    inline static const GPUFunctionDefinition mFresnelSchlick { GPUShaderDefinitions::PrimitiveTypes::mVector3, "fresnelSchlick",
        {
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mFloat, "cosTheta"},
            {GPUStorage::NONE, GPUShaderDefinitions::PrimitiveTypes::mVector3, "F0"}
        }};
};
REGISTER_CLASS(ShaderPBR)
