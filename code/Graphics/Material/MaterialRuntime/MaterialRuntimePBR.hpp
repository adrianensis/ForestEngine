#pragma once

#include "Graphics/Material/MaterialRuntime/MaterialRuntimeDefault.hpp"

// PBR METALLIC

class MaterialPropertiesBlockPBRMetallicRoughness
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

    void registerFunctionCalculatePBRSpecularGlossiness(ShaderBuilder& shaderBuilder) const;

protected:
    inline static const GPUFunctionDefinition mCalculatePBRMetallicRoughness
    {
        GPUBuiltIn::PrimitiveTypes::mVector4,
        "calculatePBRMetallicRoughness"
    };
};
