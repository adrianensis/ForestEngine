#pragma once

#include "Graphics/GPU/GPUProgramModule.h"
#include "Graphics/GPU/GPURenderPass.h"
#include "Graphics/GPU/GPUContext.hpp"

class GPUVertexInputData
{
public:
    std::vector<VkVertexInputBindingDescription> mVertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> mVertexInputAttributeDescriptions;
};

class GPUProgramPipeline
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPUProgramPipeline(GPURenderPass* vulkanRenderPass, Ptr<GPUContext> gpuContext);
    const VkPipelineLayout getPipelineLayout() const;
    const VkPipeline getPipeline() const;
    bool initialize(const GPUProgramModule& vertexShader, const GPUProgramModule& fragmentShader, VkDescriptorSetLayout descriptorSetLayout, const GPUVertexInputData& gpuVertexInputData);
    void terminate();
    void bind(const GPUCommandBuffer& vulkanCommandBuffer) const;

private:
    Ptr<GPUContext> mGPUContext;
    GPURenderPass* vulkanRenderPass;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    GPUVertexInputData mGPUVertexInputData;
};