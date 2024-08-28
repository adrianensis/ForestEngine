#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUContext.hpp"
#include "Graphics/GPU/Vulkan.h"
#include "Core/Object/ObjectBase.hpp"
#include "Graphics/GPU/GPUGraphicsPipeline.h"

class GPURenderPass;

class GPUProgramDescriptorsData
{
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
};

class GPUProgram: public ObjectBase
{    
public:

    GPUProgram();
	~GPUProgram() override = default;

    void initFromFileContents(GPURenderPass* vulkanRenderPass, const GPUProgramDescriptorsData& gpuProgramDescriptorsData, const std::vector<GPUUniformBuffer>& uniformBuffers, const std::vector<GPUVertexBuffer>& vertexInputBuffers, Ptr<GPUContext> gpuContext, const std::vector<byte>& vertex, const std::vector<byte>& fragment);
    void terminate();
    void enable() const;
    void disable() const;
private:
	u32 mProgramId = 0;

    Ptr<GPUContext> mGPUContext;

    GPUGraphicsPipeline* vulkanGraphicsPipeline;

    GPUProgramModule* vertexShader;
    GPUProgramModule* fragmentShader;
    GPUProgramDescriptorsData mGPUProgramDescriptorsData;
};
REGISTER_CLASS(GPUProgram);
