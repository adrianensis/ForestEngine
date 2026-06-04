#pragma once

#include "Core/CoreBase.hpp"
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
    GPU::u32 mMaxBindlessTextures = 1024;
    GPU::u32 mBindlessTexturesArrayBinding = 0;
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

