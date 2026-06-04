#pragma once

#include "GPU/Shader/BuiltIn/GPUShaderDefault.hpp"

// PBR METALLIC

class TextureBindingNamesPBR : public TextureBindingNames
{
public:
    inline static const std::string smNormal = "Normal";
    inline static const std::string smMetallicRoughness = "MetallicRoughness";
    inline static const std::string smShadowMap = "ShadowMap";
};

class PropertiesBlockGPUShaderPBR
{
public:
    Maths::Vector4 mBaseColor = Maths::Vector4(0,0,0,1);
    GPU::u32 mBaseColorTextureHandle = 0;
    GPU::f32 mMetallic = 0;
    GPU::f32 mRoughness = 0;
    GPU::u32 mMetallicRoughnessTextureHandle = 0;
    alignas(16) GPU::u32 mNormalTextureHandle = 0;
};
REGISTER_CLASS(PropertiesBlockGPUShaderPBR)

class GPUShaderPBR : public GPUShaderDefault
{
public:
    virtual void generateGPUShaderGenerationData(GPUShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const override;
protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateGPUShaderPropertiesBlock() override;
    virtual void registerTextures() override;
    virtual void vertexGPUShaderCalculatePositionOutput(GPUShaderBuilder& GPUShaderBuilder) const override;
    virtual void fragmentGPUShaderCode(GPUShaderBuilder& GPUShaderBuilder) const override;

    virtual void registerFragmentGPUShaderData(GPUShaderBuilder& GPUShaderBuilder, const GPUShaderCompilationData& shaderCompilationData) const override;

    void registerFunctionsGetNormalFromMap(GPUShaderBuilder& GPUShaderBuilder) const;
    void registerFunctionsShadowCalculation(GPUShaderBuilder& GPUShaderBuilder) const;
    void registerFunctionsPBRHelpers(GPUShaderBuilder& GPUShaderBuilder) const;
    void registerFunctionCalculatePBR(GPUShaderBuilder& GPUShaderBuilder) const;

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
REGISTER_CLASS(GPUShaderPBR)
