#pragma once

#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUTexture.hpp"

class GPURenderPass;

class GPUShaderTextureBinding
{
public:
    Core::HashedString mName;
    Core::WeakPtr<GPUTexture> mGPUTexture;
};

class GPUShaderDescriptorSetsData
{
public:
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::vector<GPUShaderTextureBinding> mTextureBindings;
};

class GPUShaderDescriptorSetsBindings
{
public:
    std::unordered_map<Core::HashedString, Core::u32> mBindings;
    std::unordered_map<Core::HashedString, Core::u32> mSets;
};

class GPUShaderDescriptorSets
{    
public:
    void init(const GPUShaderDescriptorSetsData& gpuGPUShaderDescriptorSetsData, Core::Ptr<GPUContext> gpuContext);
    void updateBuffers();
    void updateSamplers();
    void terminate();
private:
    Core::Ptr<GPUContext> mGPUContext;
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    GPUShaderDescriptorSetsBindings mGPUShaderDescriptorSetsBindings;
    GPUShaderDescriptorSetsData mGPUDescriptorData;
    Core::u32 mSamplersBindingIndexOffset = 0;
};
REGISTER_CLASS(GPUShaderDescriptorSets);
