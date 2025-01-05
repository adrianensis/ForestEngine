#pragma once

#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"

class GPUContext;
class GPUCommandBuffer;

class GPUCommandPool
{
public:
    GPUCommandPool(WeakPtr<GPUContext> gpuContext);
    bool init();
    void terminate();
    std::vector<GPUCommandBuffer*> allocateCommandBuffers(u32 count) const;
    void freeCommandBuffer(const GPUCommandBuffer* commandBuffer) const;
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
    VkCommandPool mVkCommandPool = VK_NULL_HANDLE;
    WeakPtr<GPUContext> mGPUContext;
public:
    CRGET(VkCommandPool)
};