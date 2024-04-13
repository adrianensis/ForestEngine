#pragma once

#include "Graphics/Material/MaterialRuntime.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class MaterialRuntimeDefault : public MaterialRuntime
{
public:
    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer,
        const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder, 
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer,
        const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;

protected:
    virtual void vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateProjectionViewMatrix(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const;

    virtual void fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const;
    virtual void fragmentShaderTexture(ShaderBuilder& shaderBuilder) const;
    virtual void fragmentShaderAlphaDiscard(ShaderBuilder& shaderBuilder) const;

    virtual void generateShaderBuilderData(MaterialRuntime::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;
    virtual void registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;
    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;

    void registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const;
};

// PBR METALLIC

class MaterialPropertiesBlockPBRMetallicRoughness
{
public:
    Vector4 mBaseColor = Vector4(0,0,0,1);
    alignas(16) Vector3 mMetallic = Vector3(0.2f,0.2f,0.2f);
    alignas(16) Vector3 mRoughness = Vector3(32,0,0);
};

class MaterialRuntimePBRMetallicRoughness : public MaterialRuntimeDefault
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
