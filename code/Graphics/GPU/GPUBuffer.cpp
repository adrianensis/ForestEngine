#include "Graphics/GPU/GPUBuffer.h"

const GPUBuffer::Config& GPUBuffer::getConfig() const {
    return config;
}

const VkBuffer GPUBuffer::getVkBuffer() const {
    return vkBuffer;
}

const VkDeviceMemory GPUBuffer::getVkDeviceMemory() const {
    return vkDeviceMemory;
}

bool GPUBuffer::init(Ptr<GPUContext> gpuContext, const Config& config)
{
    mGPUContext = gpuContext;
    this->config = config;

    VkAllocationCallbacks* allocator = VK_NULL_HANDLE;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = config.Size;
    bufferInfo.usage = config.Usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(mGPUContext->vulkanDevice->getDevice(), &bufferInfo, allocator, &vkBuffer) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan buffer");
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(mGPUContext->vulkanDevice->getDevice(), vkBuffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = mGPUContext->vulkanPhysicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, config.MemoryProperties);

    if (vkAllocateMemory(mGPUContext->vulkanDevice->getDevice(), &memoryAllocateInfo, allocator, &vkDeviceMemory) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not allocate Vulkan vkBuffer memory");
        return false;
    }

    constexpr VkDeviceSize memoryOffset = 0;
    vkBindBufferMemory(mGPUContext->vulkanDevice->getDevice(), vkBuffer, vkDeviceMemory, memoryOffset);

    LOG("Initialized Vulkan buffer");
    return true;
}

void GPUBuffer::terminate() {
    VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
    vkDestroyBuffer(mGPUContext->vulkanDevice->getDevice(), vkBuffer, allocator);
    LOG("Destroyed Vulkan buffer");
    vkFreeMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory, allocator);
    LOG("Freed Vulkan buffer memory");
    LOG("Terminated Vulkan buffer");
}

void GPUBuffer::setData(void* data) const {
    void* memory;
    constexpr VkDeviceSize memoryOffset = 0;
    constexpr VkMemoryMapFlags memoryMapFlags = 0;
    vkMapMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory, memoryOffset, config.Size, memoryMapFlags, &memory);
    std::memcpy(memory, data, config.Size);
    vkUnmapMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory);
}

void GPUBuffer::copy(const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, const GPUCommandPool& commandPool, const GPUDevice& vulkanDevice) {
    CHECK_MSG(sourceBuffer.config.Size == destinationBuffer.config.Size, "sourceBuffer.config.Size == destinationBuffer.config.Size");

    constexpr uint32_t commandBufferCount = 1;
    const std::vector<GPUCommandBuffer>& commandBuffers = commandPool.allocateCommandBuffers(commandBufferCount);
    CHECK_MSG(commandBuffers.size() == commandBufferCount, "commandBuffers.size() == commandBufferCount")

    const GPUCommandBuffer& commandBuffer = commandBuffers[0];
    commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkCommandBuffer vkCommandBuffer = commandBuffer.getVkCommandBuffer();

    VkBufferCopy copyRegion{};
    copyRegion.size = sourceBuffer.config.Size;
    constexpr uint32_t regionCount = 1;
    vkCmdCopyBuffer(vkCommandBuffer, sourceBuffer.vkBuffer, destinationBuffer.vkBuffer, regionCount, &copyRegion);

    commandBuffer.end();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    constexpr uint32_t submitCount = 1;
    VkFence fence = VK_NULL_HANDLE;
    vkQueueSubmit(vulkanDevice.getGraphicsQueue(), submitCount, &submitInfo, fence);
    vkQueueWaitIdle(vulkanDevice.getGraphicsQueue());

    commandPool.free(commandBuffer);
}