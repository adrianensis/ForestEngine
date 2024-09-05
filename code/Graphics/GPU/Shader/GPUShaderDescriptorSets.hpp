#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/Buffer/GPUUniformBuffer.hpp"
#include "Graphics/GPU/Core/GPUContext.hpp"
#include "Core/Object/ObjectBase.hpp"

class GPURenderPass;

class GPUShaderDescriptorSetsData
{
public:
    std::vector<GPUUniformBuffer> mUniformBuffers;
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
    GPUShaderDescriptorSetsData mGPUDescriptorData;
};
REGISTER_CLASS(GPUShaderDescriptorSets);
