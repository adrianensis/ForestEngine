#include "GPUCommandBuffer.h"

void GPUCommandBuffer::init(VkCommandBuffer commandBuffer)
{
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
    return true;
}

bool GPUCommandBuffer::end() const
{
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