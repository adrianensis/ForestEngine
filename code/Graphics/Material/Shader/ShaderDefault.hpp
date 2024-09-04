#pragma once

#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class ShaderDefault : public Shader
{
public:
    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUShaderDescriptorsData& gpuShaderDescriptorsData) const override;
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder, 
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUShaderDescriptorsData& gpuShaderDescriptorsData) const override;
    virtual void generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;

protected:
    virtual void registerTextures() override;
    virtual void vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const;

    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const;

    virtual void registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUShaderDescriptorsData& gpuShaderDescriptorsData) const;
    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUShaderDescriptorsData& gpuShaderDescriptorsData) const;

    void registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const;
};
REGISTER_CLASS(ShaderDefault)