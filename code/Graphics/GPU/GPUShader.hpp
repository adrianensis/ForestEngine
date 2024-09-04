#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUContext.hpp"
#include "Core/Object/ObjectBase.hpp"
#include "Graphics/GPU/GPUShaderPipeline.h"

class GPURenderPass;

class GPUShaderDescriptorsData
{
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    std::unordered_map<HashedString, u32> mUniformBufferToSet;
};

class GPUShader: public ObjectBase
{    
public:

    GPUShader();
	~GPUShader() override = default;

    void initFromFileContents(GPURenderPass* vulkanRenderPass, const GPUShaderDescriptorsData& gpuShaderDescriptorsData, const std::vector<GPUUniformBuffer>& uniformBuffers, const std::vector<GPUVertexBuffer>& vertexInputBuffers, Ptr<GPUContext> gpuContext, const std::vector<byte>& vertex, const std::vector<byte>& fragment);
    void terminate();
    void enable() const;
    void disable() const;
private:
	u32 mProgramId = 0;

    Ptr<GPUContext> mGPUContext;

    GPUShaderPipeline* gpuShaderPipeline;

    GPUShaderModule* vertexShader;
    GPUShaderModule* fragmentShader;
    GPUShaderDescriptorsData mGPUShaderDescriptorsData;
};
REGISTER_CLASS(GPUShader);
