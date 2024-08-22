#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUContext.hpp"
#include "Graphics/GPU/Vulkan.h"

class GPUVertexBuffersContainer;
class GPUUniformBuffer;

class GPUProgram: public ObjectBase
{    
public:

    GPUProgram();
	~GPUProgram() override = default;

    void initFromFilePaths(Ptr<GPUContext> gpuContext, const std::string& vertex, const std::string& fragment);
    void initFromFileContents(Ptr<GPUContext> gpuContext, const std::string& vertex, const std::string& fragment);
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
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<GPUUniformBuffer> mUniformBuffers;
    Ptr<GPUContext> mGPUContext;
};
REGISTER_CLASS(GPUProgram);
