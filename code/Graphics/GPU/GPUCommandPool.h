#pragma once

#include "Graphics/GPU/GPUPhysicalDevice.h"
#include "Graphics/GPU/GPUDevice.h"
#include "Graphics/GPU/GPUCommandBuffer.h"
#include "Graphics/GPU/GPUContext.hpp"

class GPUCommandPool {
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

private:
    GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUDevice* vulkanDevice;
    VkCommandPool vkCommandPool = VK_NULL_HANDLE;

public:
    GPUCommandPool(GPUDevice* vulkanDevice, GPUPhysicalDevice* vulkanPhysicalDevice);
    const VkCommandPool getVkCommandPool() const;
    bool initialize();
    void terminate();
    std::vector<GPUCommandBuffer> allocateCommandBuffers(uint32_t count) const;
    void free(const GPUCommandBuffer& commandBuffer) const;
};