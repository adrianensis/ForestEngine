#pragma once

#include "Core/CoreBase.hpp"
#include "GPU/Core/GPUDefinitions.h"
#include "GPU/Shader/ShaderBuilder/GPUShaderBuilder.hpp"
#include "GPU/Shader/GPUShaderPipeline.h"
#include "Core/Memory/ByteBuffer.hpp"
#include "Core/Memory/GenericObject.hpp"

class GPUMesh;
class GPUShaderManager;

class FramebufferBinding
{
public:
    Core::HashedString mSamplerName;
    Core::u32 mTextureID = 0;
};

class TextureBinding
{
public:
    Core::HashedString mPath;
    bool mIsLinearData = false;
    bool operator==(const TextureBinding& other) const { return this->mPath == other.mPath; }
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
    Core::HashedString label;
    Core::HashedString id;
    std::vector<GPUUniformBuffer> mUniformBuffers;
    GPUVertexBuffersContainer mInputVertexBuffersContainer;
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

    Core::u64 hash() const
    {
        Core::u32 shift = 0;
        Core::u64 result = 0;
        
        FOR_MAP(it, mTextureBindings)
        {
            result = result ^ (Core::u64)it->first.getHash() << (shift++);
            result = result ^ (Core::u64)it->second.mPath.getHash() << (shift++);
        }

        return result;
    }
};

class GPUShader;
class GPUShaderPropertiesInstance
{
public:
    Core::Slot mSlot;
    Core::u32 mID = 0;
    Core::WeakPtr<GPUShader> mShader;
    Core::GenericObjectBuffer mGPUShaderPropertiesBlockBuffer;
    void setDirty();
};


class GPUShaderData
{
public:
    // GPUCullFaceType mCullFaceType = GPUCullFaceType::BACK;
    bool mAllowInstances = true;
    Core::u32 mMaxInstances = 100;
    bool mIsFont = false;
    Font::FontData mFontData;
    GPUShaderTextureBindings mGPUShaderTextureBindings;

    Core::u32 getMaxInstances() const
    {
        return mAllowInstances ? mMaxInstances : 1;
    }
};

class GPUShader
{
friend class GPUShaderPropertiesInstance;
public:
    GPUShader() = default;
    virtual ~GPUShader() = default;
    virtual void init(GPUContext* gpuContext, GPUShaderManager* gpuShaderManager, const GPUShaderData& shaderData, const Core::GenericObjectBuffer& propertiesBlockGPUShaderDefault, Core::u32 id);
    void terminate();

    bool hasFramebufferBinding(Core::HashedString bindingName) const;

    void addFramebufferBinding(const FramebufferBinding& framebufferBinding);

    virtual void createVertexShader(GPUShaderBuilder& GPUShaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, Core::WeakPtr<const GPUDescriptorSet> gpuDescriptorSet) const
        {};
    virtual void createFragmentShader(GPUShaderBuilder& GPUShaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer, Core::WeakPtr<const GPUDescriptorSet> gpuDescriptorSet) const
        {};

    virtual void generateGPUShaderGenerationData(GPUShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const;
    Core::OwnerPtr<GPUShaderPipeline> compileShader(const GPUShaderCompilationData& shaderCompilationData);

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
    GPUContext* mGPUContext = nullptr;
    GPUShaderManager* mGPUShaderManager = nullptr;
    GPUStructDefinition mPropertiesBlockStructDefinition;
    GPUUniformBufferData mPropertiesBlockUniformBufferData;
    std::unordered_set<Core::HashedString> mTextures;
    std::unordered_map<Core::HashedString, FramebufferBinding> mFramebufferBindings;
    GPUShaderCompilationData mGPUShaderCompilationData;
    GPUShaderData mGPUShaderData;
    Core::u32 mID = 0;
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