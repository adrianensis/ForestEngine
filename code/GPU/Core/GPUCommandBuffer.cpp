#include "GPUCommandBuffer.h"
#include "GPU/Core/GPUContext.hpp"

void GPUCommandBuffer::init(VkCommandBuffer commandBuffer, GPUContext* gpuContext)
{
    mGPUContext = gpuContext;
    mVkCommandBuffer = commandBuffer;
}

bool GPUCommandBuffer::begin(VkCommandBufferUsageFlags usageFlags) const
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usageFlags;

    if (vkBeginCommandBuffer(mVkCommandBuffer, &beginInfo) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not begin Vulkan command buffer");
        return false;
    }

    PROFILER_GPU(mGPUContext->mTracyContext, mVkCommandBuffer, "begin buffer")

    return true;
}

bool GPUCommandBuffer::end() const
{
    {
        PROFILER_GPU(mGPUContext->mTracyContext, mVkCommandBuffer, "end buffer")
    }
    
#if defined(ENGINE_ENABLE_PROFILER)
  PROFILER_GPU_COLLECT(mGPUContext->mTracyContext, mVkCommandBuffer);
#endif
    if (vkEndCommandBuffer(mVkCommandBuffer) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not end Vulkan command buffer");
        return false;
    }
    return true;
}

void GPUCommandBuffer::reset() const
{
    VkCommandBufferResetFlags flags = 0;
    vkResetCommandBuffer(mVkCommandBuffer, flags);
}