#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "GPU/Image/GPUTexture.hpp"
#include "GPU/Shader/GPUShader.hpp"
#include "GPU/Shader/GPUShaderModule.h"
#include "Graphics/TextureAnimation/TextureAnimation.hpp"

class Shader;
class GPUMesh;

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

class ShaderPropertiesBlockNames
{
public:
    inline static const HashedString smPropertiesBlockStructName = "propertiesBlockStruct";
    inline static const HashedString smPropertiesBlockBufferName = "PropertiesBlock";
    inline static const HashedString smPropertiesBlockInstanceName = "propertiesBlock";
    inline static const HashedString smPropertiesBlockArrayName = "propertiesBlockArray";
};

class ShaderCompilationData
{
public:
    WeakPtr<const GPUMesh> mMesh;
    GPURenderPass* vulkanRenderPass;
    HashedString label;
    HashedString id;
    std::vector<GPUUniformBuffer> mUniformBuffers;
    GPUVertexBuffersContainer mInputVertexBuffersContainer;
};

class ShaderGenerationDataCommon
{
public:
    std::vector<GPUStructDefinition> mStructDefinitions;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUUniformBufferData> mUniformBuffers;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class ShaderGenerationDataVertex
{
public:
    std::vector<GPUVertexBuffer> mVertexInputs;
    std::vector<GPUVariableDefinitionData> mVertexOutputs;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUVariableDefinitionData> mSamplers;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class ShaderGenerationDataFragment
{
public:
    std::vector<GPUVariableDefinitionData> mFragmentInputs;
    std::vector<GPUVariableDefinitionData> mFragmentOutputs;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUVariableDefinitionData> mSamplers;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class ShaderGenerationData
{
public:
    ShaderGenerationDataCommon mCommonVariables;
    ShaderGenerationDataVertex mVertexVariables;
    ShaderGenerationDataFragment mFragmentVariables;
};

class ShaderData
{
public:
    GPUCullFaceType mCullFaceType = GPUCullFaceType::BACK;
    bool mAllowInstances = true;
    u32 mMaxInstances = 100;
    bool mIsFont = false;
    FontData mFontData;
    std::unordered_map<HashedString, TextureBinding> mTextureBindings;
    std::unordered_map<HashedString, TextureAnimation> mTextureAnimations;

    GenericObjectBuffer mSharedShaderPropertiesBlockBuffer;
    ClassDefinition mSharedShaderPropertiesBlockClass;

    template<class T>
    void setSharedShaderPropertiesBlock()
    {
        mSharedShaderPropertiesBlockBuffer.set<T>();
        mSharedShaderPropertiesBlockClass = ClassManager::getClassMetadata<T>().mClassDefinition;
    }

    u32 getSharedShaderPropertiesBlockBufferSize() const
    {
        return mSharedShaderPropertiesBlockBuffer.getByteBuffer().size();
    }
    bool allowInstances() const
    {
        return mAllowInstances && mMaxInstances > 0 && getSharedShaderPropertiesBlockBufferSize() > 0;
    }

    u32 getMaxInstances() const
    {
        return mAllowInstances ? mMaxInstances : 1;
    }
};

class GPURenderPass;

class Shader
{
public:
    Shader() = default;
    virtual ~Shader() = default;
    void init(const ShaderData& shaderData, u32 id);
    void terminate();
    // virtual void onPoolFree() override { terminate(); };

    void enable() const;
    void disable() const;
    bool hasFramebufferBinding(HashedString bindingName) const;

    void addFramebufferBinding(const FramebufferBinding& framebufferBinding);

    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
        {};
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
        {};

    virtual void generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;
    OwnerPtr<GPUShader> compileShader(const ShaderCompilationData& shaderCompilationData);


protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateShaderPropertiesBlock();
    virtual void registerTextures() {};

protected:
    GPUStructDefinition mPropertiesBlockStructDefinition;
    GPUUniformBufferData mPropertiesBlockUniformBufferData;
    std::unordered_set<HashedString> mTextures;
    std::unordered_map<HashedString, FramebufferBinding> mFramebufferBindings;
    ShaderCompilationData mShaderCompilationData;
    ShaderData mShaderData;
    u32 mID = 0;

public:
    CRGET(ShaderData)
    CRGET(PropertiesBlockStructDefinition)
    CRGET(PropertiesBlockUniformBufferData)
    GET(ID)
};
REGISTER_CLASS(Shader)