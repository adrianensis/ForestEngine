#pragma once

#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUTexture.hpp"

class GPURenderPass;

// TODO: rename
class GPUShaderTextureBinding
{
public:
    Core::HashedString mName;
    Core::WeakPtr<GPUTexture> mGPUTexture;
};

class GPUDescriptorsSetData
{
public:
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::vector<GPUShaderTextureBinding> mTextureBindings;
};

class GPUDescriptorsSetBindings
{
public:
    std::unordered_map<Core::HashedString, Core::u32> mBindings;
    std::unordered_map<Core::HashedString, Core::u32> mSets;
};

class GPUDescriptorsSet
{    
public:
    void init(const GPUDescriptorsSetData& gpuDescriptorsSetData, GPUContext* gpuContext);
    void update();
    void terminate();
private:
    GPUContext* mGPUContext = nullptr;
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    GPUDescriptorsSetBindings mGPUDescriptorsSetBindings;
    GPUDescriptorsSetData mGPUDescriptorData;
    Core::u32 mSamplersBindingIndexOffset = 0;
};

