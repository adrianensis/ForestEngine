#pragma once

#include "Graphics/GPU/GPUPhysicalDevice.h"
#include "Graphics/GPU/GPUDevice.h"

class GPUCommandBuffer;

class GPUCommandPool {
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

private:
    GPUDevice* vulkanDevice;
    VkCommandPool vkCommandPool = VK_NULL_HANDLE;

public:
    GPUCommandPool(GPUDevice* vulkanDevice);
    const VkCommandPool getVkCommandPool() const;
    bool initialize();
    void terminate();
    std::vector<GPUCommandBuffer*> allocateCommandBuffers(uint32_t count) const;
    void free(const GPUCommandBuffer* commandBuffer) const;
};