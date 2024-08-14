#include "GPUCommandBuffer.h"
#include "Log.h"

namespace GPUAPI {

    const VkAllocationCallbacks* GPUCommandBuffer::ALLOCATOR = VK_NULL_HANDLE;

    GPUCommandBuffer::GPUCommandBuffer(VkCommandBuffer commandBuffer) : commandBuffer(commandBuffer) {
    }

    const VkCommandBuffer GPUCommandBuffer::getVkCommandBuffer() const {
        return commandBuffer;
    }

    bool GPUCommandBuffer::begin(VkCommandBufferUsageFlags usageFlags) const {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = usageFlags;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            VD_LOG_ERROR("Could not begin Vulkan command buffer");
            return false;
        }
        return true;
    }

    bool GPUCommandBuffer::end() const {
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            VD_LOG_ERROR("Could not end Vulkan command buffer");
            return false;
        }
        return true;
    }

    void GPUCommandBuffer::reset() const {
        VkCommandBufferResetFlags flags = 0;
        vkResetCommandBuffer(commandBuffer, flags);
    }

}