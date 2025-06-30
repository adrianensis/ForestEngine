#pragma once

#include "GPU/Shader/GPUShader.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"

class TextureBindingNames
{
public:
    inline static const Core::HashedString smBaseColor = "BaseColor";
};

class PropertiesBlockGPUShaderDefault
{
public:
    Vector4 mBaseColor = Vector4(0,0,0,1);
};
REGISTER_CLASS(PropertiesBlockGPUShaderDefault)

class GPUShaderDefault : public GPUShader
{
public:
    virtual void createVertexShader(GPUShaderBuilder& GPUShaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, Core::WeakPtr<const GPUShaderDescriptorSets> gpuGPUShaderDescriptorSets) const override;
    virtual void createFragmentShader(GPUShaderBuilder& GPUShaderBuilder, 
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, Core::WeakPtr<const GPUShaderDescriptorSets> gpuGPUShaderDescriptorSets) const override;
    virtual void generateGPUShaderGenerationData(GPUShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;

protected:
    virtual void registerTextures() override;
    virtual void setSharedGPUShaderPropertiesBlock() override;
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateGPUShaderPropertiesBlock() override;
    virtual void vertexGPUShaderCalculateBoneMatrix(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexGPUShaderCalculatePositionOutput(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexGPUShaderCalculatePositionOutputCustom(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexGPUShaderCalculateNormalOutput(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexGPUShaderCalculateTextureCoordinateOutput(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexGPUShaderCalculateVertexColorOutput(GPUShaderBuilder& GPUShaderBuilder) const;
    virtual void vertexGPUShaderCalculateInstanceIdOutput(GPUShaderBuilder& GPUShaderBuilder) const;

    virtual void fragmentGPUShaderCode(GPUShaderBuilder& GPUShaderBuilder) const;

    virtual void registerVertexGPUShaderData(GPUShaderBuilder& GPUShaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, Core::WeakPtr<const GPUShaderDescriptorSets> gpuGPUShaderDescriptorSets) const;
    virtual void registerFragmentGPUShaderData(GPUShaderBuilder& GPUShaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, Core::WeakPtr<const GPUShaderDescriptorSets> gpuGPUShaderDescriptorSets) const;

    void registerFunctionCalculateBoneTransform(GPUShaderBuilder& GPUShaderBuilder) const;
};
REGISTER_CLASS(GPUShaderDefault)