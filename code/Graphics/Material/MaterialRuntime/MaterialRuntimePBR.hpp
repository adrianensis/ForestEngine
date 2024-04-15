#pragma once

#include "Graphics/Material/MaterialRuntime/MaterialRuntimeDefault.hpp"

// PBR METALLIC

class MetallicRoughness
{
public:
    Vector4 mBaseColor = Vector4(0,0,0,1);
    alignas(16) Vector3 mMetallic = Vector3(0.2f,0.2f,0.2f);
    alignas(16) Vector3 mRoughness = Vector3(32,0,0);
};

class MaterialRuntimePBR : public MaterialRuntimeDefault
{
public:
    void createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;

protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock() override;

    virtual void fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const override;
    virtual void fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const;

    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;

    void registerFunctionsPBRHelpers(ShaderBuilder& shaderBuilder) const;
    void registerFunctionCalculatePBR(ShaderBuilder& shaderBuilder) const;

protected:
    inline static const GPUFunctionDefinition mCalculatePBR { GPUBuiltIn::PrimitiveTypes::mVector4, "calculatePBR" };
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
