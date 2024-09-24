#pragma once

#include "Graphics/GPU/Core/GPUPhysicalDevice.h"
#include "Graphics/GPU/Core/GPUDevice.h"

class GPUCommandBuffer;

class GPUCommandPool
{
public:
    GPUCommandPool(GPUDevice* vulkanDevice);
    bool init();
    void terminate();
    std::vector<GPUCommandBuffer*> allocateCommandBuffers(uint32_t count) const;
    void freeCommandBuffer(const GPUCommandBuffer* commandBuffer) const;
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
    GPUDevice* vulkanDevice;
    VkCommandPool mVkCommandPool = VK_NULL_HANDLE;
public:
    CRGET(VkCommandPool)
};