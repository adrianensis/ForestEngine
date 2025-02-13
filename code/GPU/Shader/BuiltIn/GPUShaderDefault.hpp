#pragma once

#include "GPU/Shader/GPUShader.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"

class TextureBindingNames
{
public:
    inline static const HashedString smBaseColor = "BaseColor";
};

class PropertiesBlockShaderDefault
{
public:
    Vector4 mBaseColor = Vector4(0,0,0,1);
};
REGISTER_CLASS(PropertiesBlockShaderDefault)

class GPUShaderDefault : public GPUShader
{
public:
    virtual void createVertexShader(GPUShaderBuilder& GPUShaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const override;
    virtual void createFragmentShader(GPUShaderBuilder& GPUShaderBuilder, 
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const override;
    virtual void generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;

protected:
    virtual void registerTextures() override;
    virtual void setSharedShaderPropertiesBlock() override;
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateShaderPropertiesBlock() override;
    virtual void vertexShaderCalculateBoneMatrix(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutput(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutputCustom(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexShaderCalculateNormalOutput(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexShaderCalculateTextureCoordinateOutput(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexShaderCalculateVertexColorOutput(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexShaderCalculateInstanceIdOutput(GPUShaderBuilder& GPUShaderBuilder) const;

    virtual void fragmentShaderCode(GPUShaderBuilder& GPUShaderBuilder) const;

    virtual void registerVertexShaderData(GPUShaderBuilder& GPUShaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const;
    virtual void registerFragmentShaderData(GPUShaderBuilder& GPUShaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const;

    void registerFunctionCalculateBoneTransform(GPUShaderBuilder& GPUShaderBuilder) const;
};
REGISTER_CLASS(GPUShaderDefault)