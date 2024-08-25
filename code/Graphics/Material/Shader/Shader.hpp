#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/GPU/GPUTexture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUProgramModule.h"

class Material;

class FramebufferBinding
{
public:
    HashedString mSamplerName;
    u32 mTextureID = 0;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
};

class TextureBinding
{
public:
    HashedString mPath;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
};

class TextureBindingNames
{
public:
    inline static const HashedString smBaseColor = "BaseColor";
};

class ShaderPropertiesBlockNames
{
public:
    inline static const HashedString smPropertiesBlockStructName = "propertiesBlockStruct";
    inline static const HashedString smPropertiesBlockBufferName = "PropertiesBlock";
    inline static const HashedString smPropertiesBlockInstanceName = "propertiesBlock";
    inline static const HashedString smPropertiesBlockArrayName = "propertiesBlockArray";
};

class ShaderData
{
public:
    GPUStructDefinition mPropertiesBlockStructDefinition;
    GPUUniformBufferData mPropertiesBlockUniformBufferData;
    std::unordered_set<HashedString> mTextures;
    std::unordered_map<HashedString, FramebufferBinding> mFramebufferBindings;
    GPUVertexBuffersContainer mGPUVertexBuffersContainer;
};

class GPUProgramDataCommon
{
public:
    std::vector<GPUStructDefinition> mStructDefinitions;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUUniformBufferData> mUniformBuffers;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class GPUProgramDataVertex
{
public:
    std::vector<GPUVertexBuffer> mVertexInputs;
    std::vector<GPUVariableDefinitionData> mVertexOutputs;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class GPUProgramDataFragment
{
public:
    std::vector<GPUVariableDefinitionData> mFragmentInputs;
    std::vector<GPUVariableDefinitionData> mFragmentOutputs;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class GPUProgramData
{
public:
    GPUProgramDataCommon mCommonVariables;
    GPUProgramDataVertex mVertexVariables;
    GPUProgramDataFragment mFragmentVariables;
};

class GPURenderPass;

class Shader
{
public:
    Shader() = default;
    virtual ~Shader() = default;
    void init();
    void init(const ShaderData& shaderData);
    void terminate();

    void enable() const;
    void disable() const;
    bool hasFramebufferBinding(HashedString bindingName) const;

    void bindTextures(Ptr<GPUProgram> gpuProgram, const std::unordered_map<HashedString, PoolHandler<GPUTexture>>& textures) const;
    void addFramebufferBinding(const FramebufferBinding& framebufferBinding);

    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
        {};
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
        {};

    virtual void generateGPUProgramData(GPUProgramData& gpuProgramData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;
    OwnerPtr<GPUProgram> compileShader(GPURenderPass* vulkanRenderPass, HashedString label, HashedString id);


protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock();
    virtual void registerTextures() {};
    virtual void registerBuffers();

protected:
    ShaderData mShaderData;
public:

    CRGET(ShaderData)
};
REGISTER_CLASS(Shader)