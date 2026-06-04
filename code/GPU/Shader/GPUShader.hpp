#pragma once

#include "Core/CoreBase.hpp"
#include "GPU/Core/GPUDefinitions.h"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"
#include "GPU/Texture/GPUTexture.hpp"
#include "GPU/Shader/ShaderBuilder/GPUShaderBuilder.hpp"
#include "GPU/Shader/GPUShaderPipeline.h"
#include "Core/Memory/ByteBuffer.hpp"
#include "Core/Memory/GenericObject.hpp"

class GPUMesh;
class GPUShaderManager;

// TODO: is it even used?
class FramebufferBinding
{
public:
    Core::HashedString mSamplerName;
    GPU::u32 mTextureID = 0;
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
    GPUVertexBuffersContainer mInputVertexBuffersContainer;
    const GPUDescriptorSet* mGPUDescriptorSetGlobal = nullptr;
    const GPUDescriptorSet* mGPUDescriptorSetLocal = nullptr;
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
    std::unordered_map<Core::HashedString, GPUTextureHandle> mTextureBindings;

    // bool operator==(const GPUShaderTextureBindings& other) const
    // {
    //     if(this == &other) {return true;}
    //     return mTextureBindings == other.mTextureBindings;
    // }

    // GPU::u64 hash() const
    // {
    //     GPU::u32 shift = 0;
    //     GPU::u64 result = 0;
        
    //     FOR_MAP(it, mTextureBindings)
    //     {
    //         result = result ^ (GPU::u64)it->first.getHash() << (shift++);
    //         result = result ^ (GPU::u64)it->second.mPath.getHash() << (shift++);
    //     }

    //     return result;
    // }
};

class GPUShader;
class GPUShaderPropertiesInstance
{
public:
    Core::Slot mSlot;
    GPU::u32 mID = 0;
    Core::WeakPtr<GPUShader> mShader;
    Core::GenericObjectBuffer mGPUShaderPropertiesBlockBuffer;
    void setDirty();
};


class GPUShaderData
{
public:
    // GPUCullFaceType mCullFaceType = GPUCullFaceType::BACK;
    bool mAllowInstances = true;
    GPU::u32 mMaxInstances = 100;
    std::vector<GPUTextureBinding> mTextureBindings;

    GPU::u32 getMaxInstances() const
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
    virtual void init(GPUContext* gpuContext, GPUShaderManager* gpuShaderManager, const GPUShaderData& shaderData, const Core::GenericObjectBuffer& propertiesBlockGPUShaderDefault, GPU::u32 id);
    void terminate();

    bool hasFramebufferBinding(Core::HashedString bindingName) const;

    void addFramebufferBinding(const FramebufferBinding& framebufferBinding);

    virtual void createVertexShader(GPUShaderBuilder& GPUShaderBuilder,
        const GPUShaderCompilationData& shaderCompilationData) const
        {};
    virtual void createFragmentShader(GPUShaderBuilder& GPUShaderBuilder,
        const GPUShaderCompilationData& shaderCompilationData) const
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
    GPUShaderData mGPUShaderData;
    GPU::u32 mID = 0;
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