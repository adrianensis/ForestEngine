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

class GPUDescriptorSetData
{
public:
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::vector<GPUShaderTextureBinding> mTextureBindings;
};

class GPUDescriptorSetBindings
{
public:
    std::unordered_map<Core::HashedString, Core::u32> mBindings;
    std::unordered_map<Core::HashedString, Core::u32> mSets;
};

// TODO: Refactor, separate Layout and Pool, for bindless architecture
class GPUDescriptorSet
{    
public:
    void init(const GPUDescriptorSetData& gpuDescriptorSetData, GPUContext* gpuContext);
    void update();
    void terminate();
private:
    GPUContext* mGPUContext = nullptr;
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    GPUDescriptorSetBindings mGPUDescriptorSetBindings;
    GPUDescriptorSetData mGPUDescriptorData;
    Core::u32 mSamplersBindingIndexOffset = 0;
};

