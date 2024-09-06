#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/Buffer/GPUUniformBuffer.hpp"
#include "Graphics/GPU/Core/GPUContext.hpp"
#include "Graphics/GPU/Image/GPUTexture.hpp"
#include "Core/Object/ObjectBase.hpp"

class GPURenderPass;

class GPUShaderSamplerBinding
{
public:
    PoolHandler<GPUTexture> mGPUTexture;
};

class GPUShaderDescriptorSetsData
{
public:
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::vector<GPUShaderSamplerBinding> mSamplerBindings;
};

class GPUShaderDescriptorSets: public ObjectBase
{    
public:
    void init(const GPUShaderDescriptorSetsData& gpuShaderDescriptorSetsData, Ptr<GPUContext> gpuContext);
    void terminate();
private:
    Ptr<GPUContext> mGPUContext;
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    std::unordered_map<HashedString, u32> mUniformBufferToSet;
    std::unordered_map<HashedString, u32> mSamplersToSet;
    GPUShaderDescriptorSetsData mGPUDescriptorData;
};
REGISTER_CLASS(GPUShaderDescriptorSets);
