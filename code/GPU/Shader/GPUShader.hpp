#pragma once

#include "Core/Minimal.hpp"
#include "GPU/Shader/ShaderBuilder/GPUShaderBuilder.hpp"
#include "GPU/Shader/GPUShaderPipeline.h"

class GPUMesh;

class FramebufferBinding
{
public:
    Core::HashedString mSamplerName;
    u32 mTextureID = 0;
};

class TextureBinding
{
public:
    Core::HashedString mPath;
    bool operator==(const TextureBinding& other) const { return this->mPath == other.mPath; }
};

class GPUShaderStencilData
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

    bool operator==(const GPUShaderStencilData& other) const
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

class GPUShaderPropertiesBlockNames
{
public:
    inline static const Core::HashedString smPropertiesBlockStructName = "propertiesBlockStruct";
    inline static const Core::HashedString smPropertiesBlockBufferName = "PropertiesBlock";
    inline static const Core::HashedString smPropertiesBlockInstanceName = "propertiesBlock";
    inline static const Core::HashedString smPropertiesBlockArrayName = "propertiesBlockArray";
};

class GPUShaderCompilationData
{
public:
    WeakPtr<const GPUMesh> mMesh;
    Ptr<GPURenderPass> mRenderPass;
    Core::HashedString label;
    Core::HashedString id;
    std::vector<GPUUniformBuffer> mUniformBuffers;
    GPUVertexBuffersContainer mInputVertexBuffersContainer;
    GPUShaderPipelineDepthStencilData mGPUShaderPipelineDepthStencilData;
};

class GPUShaderGenerationDataCommon
{
public:
    std::vector<GPUStructDefinition> mStructDefinitions;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUUniformBufferData> mUniformBuffers;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class GPUShaderGenerationDataVertex
{
public:
    std::vector<GPUVertexBuffer> mVertexInputs;
    std::vector<GPUVariableDefinitionData> mVertexOutputs;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUVariableDefinitionData> mSamplers;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class GPUShaderGenerationDataFragment
{
public:
    std::vector<GPUVariableDefinitionData> mFragmentInputs;
    std::vector<GPUVariableDefinitionData> mFragmentOutputs;
    std::vector<GPUVariableDefinitionData> mUniforms;
    std::vector<GPUVariableDefinitionData> mSamplers;
    std::vector<GPUVariableDefinitionData> mConsts;
};

class GPUShaderGenerationData
{
public:
    GPUShaderGenerationDataCommon mCommonVariables;
    GPUShaderGenerationDataVertex mVertexVariables;
    GPUShaderGenerationDataFragment mFragmentVariables;
};
class GPUShaderTextureBindings
{
public:
    std::unordered_map<Core::HashedString, TextureBinding> mTextureBindings;

    bool operator==(const GPUShaderTextureBindings& other) const
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

class GPUShader;
class GPUShaderPropertiesInstance
{
public:
    Core::Slot mSlot;
    u32 mID = 0;
    WeakPtr<GPUShader> mShader;
    Core::GenericObjectBuffer mGPUShaderPropertiesBlockBuffer;
    void setDirty();
};
REGISTER_CLASS(GPUShaderPropertiesInstance);

class GPUShaderData
{
public:
    // GPUCullFaceType mCullFaceType = GPUCullFaceType::BACK;
    bool mAllowInstances = true;
    u32 mMaxInstances = 100;
    bool mIsFont = false;
    FontData mFontData;
    GPUShaderTextureBindings mGPUShaderTextureBindings;

    u32 getMaxInstances() const
    {
        return mAllowInstances ? mMaxInstances : 1;
    }
};

class GPUShader
{
public:
    GPUShader() = default;
    virtual ~GPUShader() = default;
    virtual void init(const GPUShaderData& shaderData, const Core::GenericObjectBuffer& propertiesBlockGPUShaderDefault, u32 id);
    void terminate();

    bool hasFramebufferBinding(Core::HashedString bindingName) const;

    void addFramebufferBinding(const FramebufferBinding& framebufferBinding);

    virtual void createVertexShader(GPUShaderBuilder& GPUShaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuGPUShaderDescriptorSets) const
        {};
    virtual void createFragmentShader(GPUShaderBuilder& GPUShaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuGPUShaderDescriptorSets) const
        {};

    virtual void generateGPUShaderGenerationData(GPUShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;
    OwnerPtr<GPUShaderPipeline> compileShader(const GPUShaderCompilationData& shaderCompilationData);

    bool allowInstances() const
    {
        return mGPUShaderData.mAllowInstances && mGPUShaderData.mMaxInstances > 0 && getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size() > 0;
    }
protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateGPUShaderPropertiesBlock();
    virtual void registerTextures() {};
    
    virtual void setSharedGPUShaderPropertiesBlock()
    {
        CHECK_MSG(false, "Implement!")
        // mSharedGPUShaderPropertiesBlockBuffer.set<T>();
        // mSharedGPUShaderPropertiesBlockClass = Core::ClassManager::getClassMetadata<T>().mClassDefinition;
    }

protected:
    GPUStructDefinition mPropertiesBlockStructDefinition;
    GPUUniformBufferData mPropertiesBlockUniformBufferData;
    std::unordered_set<Core::HashedString> mTextures;
    std::unordered_map<Core::HashedString, FramebufferBinding> mFramebufferBindings;
    GPUShaderCompilationData mGPUShaderCompilationData;
    GPUShaderData mGPUShaderData;
    u32 mID = 0;
    Core::GenericObjectBuffer mSharedGPUShaderPropertiesBlockBuffer;
    Core::ClassDefinition mSharedGPUShaderPropertiesBlockClass;

public:
    CRGET(GPUShaderData)
    CRGET(SharedGPUShaderPropertiesBlockBuffer)
    CRGET(SharedGPUShaderPropertiesBlockClass)
    CRGET(PropertiesBlockStructDefinition)
    CRGET(PropertiesBlockUniformBufferData)
    GET(ID)
};
REGISTER_CLASS(GPUShader)