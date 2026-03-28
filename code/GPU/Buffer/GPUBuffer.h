#pragma once

#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUCommandPool.h"
#include "GPU/Core/GPUContext.hpp"

class GPUBufferData
{
public:
    VkDeviceSize Size = 0;
    VkBufferUsageFlags Usage = 0;
    VkMemoryPropertyFlags MemoryProperties = 0;
};

class GPUBuffer
{
public:
    bool init(GPUContext* gpuContext, const GPUBufferData& gpuBufferData);
    void resize(Core::u32 size);
    void terminate();
    void setData(const void* data, Core::u32 size) const;
    static void copy(GPUContext* gpuContext, const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, VkCommandBuffer* vkCommandBuffer);

private:
    GPUContext* mGPUContext = nullptr;
    GPUBufferData mGPUBufferData;
    VkBuffer mVkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mVkDeviceMemory = VK_NULL_HANDLE;
    bool mInit = false;
public:
    CRGET(GPUBufferData)
    CRGET(VkBuffer)
};
