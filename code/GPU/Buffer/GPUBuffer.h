#pragma once

#include "GPU/Core/GPUMemoryAllocator.hpp"
#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUCommandPool.h"
#include "GPU/Core/GPUContext.hpp"

class GPUBufferData
{
public:
    VkDeviceSize Size = 0;
    VkBufferUsageFlags Usage = 0;
    VmaMemoryUsage MemoryUsage = VMA_MEMORY_USAGE_AUTO;
};

class GPUBuffer
{
public:
    bool init(GPUContext* gpuContext, const GPUBufferData& gpuBufferData);
    // void resize(GPU::u32 size);
    void terminate();
    void setData(const void* data, GPU::u32 size);
    static void copy(GPUContext* gpuContext, const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, VkCommandBuffer* vkCommandBuffer);

private:
    GPUContext* mGPUContext = nullptr;
    GPUBufferData mGPUBufferData;
    VkBuffer mVkBuffer = VK_NULL_HANDLE;
    VmaAllocation mAllocation = VK_NULL_HANDLE;
    bool mInit = false;
public:
    const auto& getGPUBufferData() const { return mGPUBufferData; }
    auto getVkBuffer() const { return mVkBuffer; }
};
