#pragma once

#include "Graphics/Material/MaterialRuntime.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class Material;

// PBR SPECULAR

class MaterialLightingModelPBRSpecularGlossiness
{
public:
    Vector3 mDiffuse = Vector3(1,1,1);
    alignas(16) Vector3 mSpecular = Vector3(0.2f,0.2f,0.2f);
    alignas(16) Vector3 mGlossiness = Vector3(32,0,0);
};

class MaterialPropertiesBlockPBRSpecularGlossiness
{
public:
    MaterialLightingModelPBRSpecularGlossiness mMaterialLightingModel;
};

class MaterialRuntimePBRSpecularGlossiness : public MaterialRuntime
{
public:
    void createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;

protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock() override;

    virtual void fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const override;
    virtual void fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const override;

    virtual void generateShaderBuilderData(MaterialRuntime::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;
    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;

    void registerFunctionCalculatePBRSpecularGlossiness(ShaderBuilder& shaderBuilder) const;

protected:
	GPUStructDefinition mLightingModelStructDefinition = 
    {
        "materialLightingModel",
        {
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Diffuse"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Specular"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Glossiness"},
        }
    };

    inline static const GPUFunctionDefinition mCalculatePBRSpecularGlossiness
    {
        GPUBuiltIn::PrimitiveTypes::mVector4,
        "calculatePBRSpecularGlossiness"
    };
};

// PBR METALLIC

class MaterialLightingModelPBRMetallicRoughness
{
public:
    Vector4 mBaseColor = Vector4(1,1,1,1);
    alignas(16) Vector3 mMetallic = Vector3(0.2f,0.2f,0.2f);
    alignas(16) Vector3 mRoughness = Vector3(32,0,0);
};

class MaterialPropertiesBlockPBRMetallicRoughness
{
public:
    MaterialLightingModelPBRMetallicRoughness mMaterialLightingModel;
};

class MaterialRuntimePBRMetallicRoughness : public MaterialRuntime
{
public:
    void createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;

protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock() override;

    virtual void fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const override;
    virtual void fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const override;

    virtual void generateShaderBuilderData(MaterialRuntime::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;
    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;

    void registerFunctionCalculatePBRSpecularGlossiness(ShaderBuilder& shaderBuilder) const;

protected:
	GPUStructDefinition mLightingModelStructDefinition = 
    {
        "materialLightingModel",
        {
            {GPUBuiltIn::PrimitiveTypes::mVector4, "BaseColor"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Metallic"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Roughness"}
        }
    };

    inline static const GPUFunctionDefinition mCalculatePBRMetallicRoughness
    {
        GPUBuiltIn::PrimitiveTypes::mVector4,
        "calculatePBRMetallicRoughness"
    };
};
