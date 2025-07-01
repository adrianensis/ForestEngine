#pragma once

#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"

class GPUContext;
class GPUCommandBuffer;

class GPUCommandPool
{
public:
    bool init(Core::Ptr<GPUContext> gpuContext, VkCommandPoolCreateFlags creationFlags);
    void terminate();
    std::vector<GPUCommandBuffer> allocateCommandBuffers(Core::u32 count) const;
    void freeCommandBuffer(const GPUCommandBuffer& commandBuffer) const;
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
    VkCommandPool mVkCommandPool = VK_NULL_HANDLE;
    Core::Ptr<GPUContext> mGPUContext;
public:
    CRGET(VkCommandPool)
};