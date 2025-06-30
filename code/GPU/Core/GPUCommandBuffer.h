#pragma once

#include <vulkan/vulkan.h>

#include "Core/Minimal.hpp"

class GPUContext;

class GPUCommandBuffer
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    void init(VkCommandBuffer commandBuffer, Core::Ptr<GPUContext> gpuContext);
    bool begin(VkCommandBufferUsageFlags usageFlags = 0) const;
    bool end() const;
    void reset() const;

private:
    VkCommandBuffer mVkCommandBuffer = VK_NULL_HANDLE;
    Core::Ptr<GPUContext> mGPUContext;
public:
    CRGET(VkCommandBuffer)
};