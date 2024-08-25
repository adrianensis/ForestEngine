#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUContext.hpp"
#include "Graphics/GPU/Vulkan.h"
#include "Core/Object/ObjectBase.hpp"
#include "Graphics/GPU/GPUGraphicsPipeline.h"

class GPUVertexBuffersContainer;
class GPUUniformBuffer;
class GPURenderPass;

class GPUProgram: public ObjectBase
{    
public:

    GPUProgram();
	~GPUProgram() override = default;

    void initFromFileContents(GPURenderPass* vulkanRenderPass, Ptr<GPUContext> gpuContext, const std::string& vertex, const std::string& fragment);
    void terminate();
    void enable() const;
    void disable() const;
    template<class T>
    void bindUniformValue(const HashedString& name, const T& value)
    {
//        GET_SYSTEM(GPUInterface).bindUniformValue<T>(mProgramId, name, value);
    }
    void bindUniformBuffer(const GPUUniformBuffer& uniformBuffer);
    void createDescriptors();
private:
	u32 mProgramId = 0;

    Ptr<GPUContext> mGPUContext;

    GPUGraphicsPipeline* vulkanGraphicsPipeline;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    GPUProgramModule* vertexShader;
    GPUProgramModule* fragmentShader;
    std::vector<GPUUniformBuffer> mUniformBuffers;
};
REGISTER_CLASS(GPUProgram);
