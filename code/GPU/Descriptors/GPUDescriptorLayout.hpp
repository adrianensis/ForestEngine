#pragma once

#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUTexture.hpp"

// TODO: rename GPUTextureBinding to just GPUTextureBinding (?)
class GPUTextureBinding
{
public:
    Core::HashedString mName;
    Core::WeakPtr<GPUTexture> mGPUTexture;
};

class GPUDescriptorLayoutData
{
public:
    // TODO: Stop copying GPUUniformBuffers everywhere! Just pass data around!
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::vector<GPUTextureBinding> mTextureBindings;
    bool mIsBindless = false;
};

// TODO: Refactor, separate Layout and Pool, for bindless architecture
class GPUDescriptorLayout
{    
public:
    void init(const GPUDescriptorLayoutData& gpuDescriptorLayoutData, GPUContext* gpuContext);
    void terminate();
private:
    GPUContext* mGPUContext = nullptr;
public:
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    GPUDescriptorLayoutData mGPUDescriptorLayoutData;
};

