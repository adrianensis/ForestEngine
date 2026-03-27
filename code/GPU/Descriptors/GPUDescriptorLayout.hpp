#pragma once

#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Texture/GPUTexture.hpp"

class GPUTextureBinding
{
public:
    Core::HashedString mName;
    GPUTextureHandle mGPUTextureHandle;
};

class GPUDescriptorLayoutData
{
public:
    // TODO: Stop copying GPUUniformBuffers everywhere! Just pass data around!
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::vector<GPUTextureBinding> mTextureBindings;
    bool mIsBindless = false;
};

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

