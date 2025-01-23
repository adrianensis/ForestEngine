#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "GPU/Image/GPUTexture.hpp"
#include "GPU/Shader/GPUShaderPipeline.h"
#include "GPU/Shader/GPUShaderModule.h"
#include "Graphics/TextureAnimation/TextureAnimation.hpp"

class GPUMesh;

class FramebufferBinding
{
public:
    HashedString mSamplerName;
    u32 mTextureID = 0;
};

class TextureBinding
{
public:
    HashedString mPath;
    bool operator==(const TextureBinding& other) const { return this->mPath == other.mPath; }
};

class ShaderStencilData
{
public:
    bool mUseStencil = false;
    // aka ref
    u32 mStencilValue = 0;
    GPUStencilFunction mStencilFunction = GPUStencilFunction::NOTEQUAL;
    GPUStencilOp mStencilPassOp = GPUStencilOp::KEEP;
    GPUStencilOp mStencilFailOp = GPUStencilOp::KEEP;
    GPUStencilOp mDepthFailOp = GPUStencilOp::KEEP;
    u64 mParentId = 0;
    u64 mId = 0;

    bool operator==(const ShaderStencilData& other) const
    {
        if(this == &other) {return true;}
        return
        mUseStencil == other.mUseStencil and
        mStencilValue == other.mStencilValue and
        mParentId == other.mParentId and
        //mId == other.mId && 
        mStencilFunction == other.mStencilFunction;
    }

    u64 hash() const
    {
        u32 shift = 0;
        u64 result = (u64)mUseStencil << (shift++);
        result = result ^ (u64)mStencilValue << (shift++);
        result = result ^ static_cast<u64>(mStencilFunction) << (shift++);
        result = result ^ (u64)mParentId << (shift++); /*^ (u64)mId*/;
        return result;
    }
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
    GPURenderPass* mRenderPass = nullptr;
    HashedString label;
    HashedString id;
    std::vector<GPUUniformBuffer> mUniformBuffers;
    GPUVertexBuffersContainer mInputVertexBuffersContainer;
    GPUShaderPipelineDepthStencilData mGPUShaderPipelineDepthStencilData;
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
class ShaderTextureBindings
{
public:
    std::unordered_map<HashedString, TextureBinding> mTextureBindings;

    bool operator==(const ShaderTextureBindings& other) const
    {
        if(this == &other) {return true;}
        return mTextureBindings == other.mTextureBindings;
    }

    u64 hash() const
    {
        u32 shift = 0;
        u64 result = 0;
        
        FOR_MAP(it, mTextureBindings)
        {
            result = result ^ (u64)it->first.getHash() << (shift++);
            result = result ^ (u64)it->second.mPath.getHash() << (shift++);
        }

        return result;
    }
};

class Shader;
class ShaderInstance
{
public:
    Slot mSlot;
    u32 mID = 0;
    WeakPtr<Shader> mShader;
    GenericObjectBuffer mShaderPropertiesBlockBuffer;
    void setDirty();
};
REGISTER_CLASS(ShaderInstance);

class ShaderData
{
public:
    // GPUCullFaceType mCullFaceType = GPUCullFaceType::BACK;
    bool mAllowInstances = true;
    u32 mMaxInstances = 100;
    bool mIsFont = false;
    FontData mFontData;
    ShaderTextureBindings mShaderTextureBindings;
    // std::unordered_map<HashedString, TextureAnimation> mTextureAnimations;

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

class Shader: public EnablePtrToThis
{
public:
    Shader() = default;
    virtual ~Shader() = default;
    virtual void init(const ShaderData& shaderData, u32 id);
    void terminate();
    // virtual void onPoolFree() override { terminate(); };

    bool hasFramebufferBinding(HashedString bindingName) const;

    void addFramebufferBinding(const FramebufferBinding& framebufferBinding);

    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
        {};
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
        {};

    virtual void generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;
    OwnerPtr<GPUShaderPipeline> compileShader(const ShaderCompilationData& shaderCompilationData);

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