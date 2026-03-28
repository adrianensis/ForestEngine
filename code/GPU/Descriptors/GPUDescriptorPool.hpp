#pragma once

#include "GPU/Core/GPUContext.hpp"

class GPUDescriptorPoolData
{
public:
    bool mUseBindlessTextures = false;
    Core::u32 mMaxBindlessTextures = 1024;
    Core::u32 mMaxStorageBuffersPerSet = 5;
    Core::u32 mMaxUniformBuffersPerSet = 5;
    Core::u32 mMaxSampledImagesPerSet = 5;
    Core::u32 mMaxSets = 100;
};

class GPUDescriptorPool
{    
public:
    void init(GPUContext* gpuContext, const GPUDescriptorPoolData& gpuDescriptorPoolData);
    void terminate();
private:
    GPUContext* mGPUContext = nullptr;
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    GPUDescriptorPoolData mGPUDescriptorPoolData;
};

