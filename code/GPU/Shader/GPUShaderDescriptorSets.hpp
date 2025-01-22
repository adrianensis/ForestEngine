#pragma once

#include "Core/Minimal.hpp"
#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUTexture.hpp"

class GPURenderPass;

class GPUShaderTextureBinding
{
public:
    HashedString mName;
    WeakPtr<GPUTexture> mGPUTexture;
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
    std::unordered_map<HashedString, u32> mBindings;
    std::unordered_map<HashedString, u32> mSets;
};

class GPUShaderDescriptorSets
{    
public:
    void init(const GPUShaderDescriptorSetsData& gpuShaderDescriptorSetsData, Ptr<GPUContext> gpuContext);
    void update();
    void terminate();
private:
    Ptr<GPUContext> mGPUContext;
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    GPUShaderDescriptorSetsBindings mGPUShaderDescriptorSetsBindings;
    GPUShaderDescriptorSetsData mGPUDescriptorData;
    u32 mSamplersBindingIndexOffset = 0;
};
REGISTER_CLASS(GPUShaderDescriptorSets);
