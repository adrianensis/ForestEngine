#pragma once

#include "Graphics/Shader/Shader.hpp"
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

class ShaderDefault : public Shader
{
public:
    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const override;
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder, 
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const override;
    virtual void generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;

protected:
    virtual void registerTextures() override;
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateShaderPropertiesBlock() override;
    virtual void vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const;

    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const;

    virtual void registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const;
    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const;

    void registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const;
};
REGISTER_CLASS(ShaderDefault)