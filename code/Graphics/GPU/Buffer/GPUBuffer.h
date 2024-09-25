#pragma once

#include "Graphics/GPU/Core/GPUPhysicalDevice.h"
#include "Graphics/GPU/Core/GPUDevice.h"
#include "Graphics/GPU/Core/GPUCommandPool.h"
#include "Graphics/GPU/Core/GPUContext.hpp"

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
    bool init(Ptr<GPUContext> gpuContext, const GPUBufferData& gpuBufferData);
    void terminate();
    void setData(const void* data) const;
    static void copy(const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, const GPUCommandPool& commandPool, const GPUDevice& vulkanDevice);

private:
    Ptr<GPUContext> mGPUContext;
    GPUBufferData mGPUBufferData;
    VkBuffer mVkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mVkDeviceMemory = VK_NULL_HANDLE;
    bool mInit = false;
public:
    CRGET(GPUBufferData)
    CRGET(VkBuffer)
    CRGET(VkDeviceMemory)
};
