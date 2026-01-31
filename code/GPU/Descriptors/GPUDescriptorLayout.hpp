#pragma once

#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUTexture.hpp"

// TODO: rename GPUShaderTextureBinding to just GPUTextureBinding (?)
class GPUShaderTextureBinding
{
public:
    Core::HashedString mName;
    Core::WeakPtr<GPUTexture> mGPUTexture;
};

class GPUDescriptorLayoutData
{
public:
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::vector<GPUShaderTextureBinding> mTextureBindings;
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

