#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/Vulkan/Vulkan.h"

class GPUVertexBuffersContainer;
class GPUSharedBuffer;

class GPUProgram: public ObjectBase
{    
public:

    GPUProgram();
	~GPUProgram() override = default;

    void initFromFilePaths(const std::string& vertex, const std::string& fragment);
    void initFromFileContents(const std::string& vertex, const std::string& fragment);
    void terminate();
    void enable() const;
    void disable() const;
    template<class T>
    void bindUniformValue(const HashedString& name, const T& value)
    {
        GET_SYSTEM(GPUInterface).bindUniformValue<T>(mProgramId, name, value);
    }
    void bindSharedBuffer(const GPUSharedBuffer& sharedBuffer);
    void createDescriptors();

private:
	u32 mProgramId = 0;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<GPUSharedBuffer> mSharedBuffers;
};
REGISTER_CLASS(GPUProgram);
