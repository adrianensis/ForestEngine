#include "Graphics/GPU/Buffer/GPUBuffer.h"
#include "Graphics/GPU/Core/GPUCommandBuffer.h"

const VkBuffer GPUBuffer::getVkBuffer() const {
    return vkBuffer;
}

const VkDeviceMemory GPUBuffer::getVkDeviceMemory() const {
    return vkDeviceMemory;
}

bool GPUBuffer::init(Ptr<GPUContext> gpuContext, const GPUBufferData& gpuBufferData)
{
    mGPUContext = gpuContext;
    mGPUBufferData = gpuBufferData;

    VkAllocationCallbacks* allocator = VK_NULL_HANDLE;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = mGPUBufferData.Size;
    bufferInfo.usage = mGPUBufferData.Usage;
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
    memoryAllocateInfo.memoryTypeIndex = mGPUContext->vulkanPhysicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, mGPUBufferData.MemoryProperties);

    if (vkAllocateMemory(mGPUContext->vulkanDevice->getDevice(), &memoryAllocateInfo, allocator, &vkDeviceMemory) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not allocate Vulkan vkBuffer memory");
        return false;
    }

    constexpr VkDeviceSize memoryOffset = 0;
    vkBindBufferMemory(mGPUContext->vulkanDevice->getDevice(), vkBuffer, vkDeviceMemory, memoryOffset);

    mInit = true;
    LOG("Initialized Vulkan buffer");
    return true;
}

void GPUBuffer::terminate() {
    if(mInit)
    {
        VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
        vkDestroyBuffer(mGPUContext->vulkanDevice->getDevice(), vkBuffer, allocator);
        LOG("Destroyed Vulkan buffer");
        vkFreeMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory, allocator);
        LOG("Freed Vulkan buffer memory");
        LOG("Terminated Vulkan buffer");
        mInit = false;
    }
}

void GPUBuffer::setData(const void* data) const {
    void* memory;
    constexpr VkDeviceSize memoryOffset = 0;
    constexpr VkMemoryMapFlags memoryMapFlags = 0;
    vkMapMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory, memoryOffset, mGPUBufferData.Size, memoryMapFlags, &memory);
    std::memcpy(memory, data, mGPUBufferData.Size);
    vkUnmapMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory);
}

void GPUBuffer::copy(const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, const GPUCommandPool& commandPool, const GPUDevice& vulkanDevice) {
    CHECK_MSG(sourceBuffer.mGPUBufferData.Size <= destinationBuffer.mGPUBufferData.Size, "sourceBuffer.config.Size == destinationBuffer.config.Size");

    constexpr uint32_t commandBufferCount = 1;
    const std::vector<GPUCommandBuffer*> commandBuffers = commandPool.allocateCommandBuffers(commandBufferCount);
    CHECK_MSG(commandBuffers.size() == commandBufferCount, "commandBuffers.size() == commandBufferCount")

    const GPUCommandBuffer* commandBuffer = commandBuffers[0];
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkCommandBuffer vkCommandBuffer = commandBuffer->getVkCommandBuffer();

    VkBufferCopy copyRegion{};
    copyRegion.size = sourceBuffer.mGPUBufferData.Size;
    constexpr uint32_t regionCount = 1;
    vkCmdCopyBuffer(vkCommandBuffer, sourceBuffer.vkBuffer, destinationBuffer.vkBuffer, regionCount, &copyRegion);

    commandBuffer->end();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    constexpr uint32_t submitCount = 1;
    VkFence fence = VK_NULL_HANDLE;
    vkQueueSubmit(vulkanDevice.getGraphicsQueue(), submitCount, &submitInfo, fence);
    vkQueueWaitIdle(vulkanDevice.getGraphicsQueue());

    commandPool.freeCommandBuffer(commandBuffer);
}