#include "GPUCommandBuffer.h"

void GPUCommandBuffer::init(VkCommandBuffer commandBuffer)
{
    mCommandBuffer = commandBuffer;
}

const VkCommandBuffer GPUCommandBuffer::getVkCommandBuffer() const {
    return mCommandBuffer;
}

bool GPUCommandBuffer::begin(VkCommandBufferUsageFlags usageFlags) const {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usageFlags;

    if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not begin Vulkan command buffer");
        return false;
    }
    return true;
}

bool GPUCommandBuffer::end() const {
    if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not end Vulkan command buffer");
        return false;
    }
    return true;
}

void GPUCommandBuffer::reset() const {
    VkCommandBufferResetFlags flags = 0;
    vkResetCommandBuffer(mCommandBuffer, flags);
}