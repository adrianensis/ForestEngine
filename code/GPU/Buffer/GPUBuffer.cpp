#include "GPU/Buffer/GPUBuffer.h"
#include "GPU/Core/GPUCommandBuffer.h"
#include "GPU/Core/GPULog.h"

bool GPUBuffer::init(Ptr<GPUContext> gpuContext, const GPUBufferData& gpuBufferData)
{
    PROFILER_CPU_NAMED(buffer_init)
    mGPUContext = gpuContext;
    mGPUBufferData = gpuBufferData;

    VkAllocationCallbacks* allocator = VK_NULL_HANDLE;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = mGPUBufferData.Size;
    bufferInfo.usage = mGPUBufferData.Usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(mGPUContext->vulkanDevice->getDevice(), &bufferInfo, allocator, &mVkBuffer) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan buffer");
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(mGPUContext->vulkanDevice->getDevice(), mVkBuffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = mGPUContext->vulkanPhysicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, mGPUBufferData.MemoryProperties);

    if (vkAllocateMemory(mGPUContext->vulkanDevice->getDevice(), &memoryAllocateInfo, allocator, &mVkDeviceMemory) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not allocate Vulkan vkBuffer memory");
        return false;
    }

    constexpr VkDeviceSize memoryOffset = 0;
    vkBindBufferMemory(mGPUContext->vulkanDevice->getDevice(), mVkBuffer, mVkDeviceMemory, memoryOffset);

    mInit = true;
    GPU_LOG("Initialized Vulkan buffer");
    return true;
}

void GPUBuffer::terminate()
{
    PROFILER_CPU_NAMED(buffer_terminate)
    if(mInit)
    {
        mGPUContext->waitForFence(mGPUContext->currentFrame - 1);

        VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
        vkDestroyBuffer(mGPUContext->vulkanDevice->getDevice(), mVkBuffer, allocator);
        GPU_LOG("Destroyed Vulkan buffer");
        vkFreeMemory(mGPUContext->vulkanDevice->getDevice(), mVkDeviceMemory, allocator);
        GPU_LOG("Freed Vulkan buffer memory");
        GPU_LOG("Terminated Vulkan buffer");
        mInit = false;
    }
}

void GPUBuffer::resize(u32 size)
{
    mGPUContext->waitForFence(mGPUContext->currentFrame - 1);

    terminate();

    GPUBufferData gpuBufferData = mGPUBufferData;
    gpuBufferData.Size = size;
    if(!init(mGPUContext, gpuBufferData))
    {
        CHECK_MSG(false,"Could not resize vertex buffer");
    }
}

void GPUBuffer::setData(const void* data, u32 size) const
{
    PROFILER_CPU_NAMED(buffer_set_data)
    CHECK_MSG(size > 0, "size > 0")
    CHECK_MSG(size <= mGPUBufferData.Size, "size <= mGPUBufferData.Size")

    void* memory = nullptr;
    constexpr VkDeviceSize memoryOffset = 0;
    constexpr VkMemoryMapFlags memoryMapFlags = 0;
    vkMapMemory(mGPUContext->vulkanDevice->getDevice(), mVkDeviceMemory, memoryOffset, size, memoryMapFlags, &memory);
    std::memcpy(memory, data, size);
    vkUnmapMemory(mGPUContext->vulkanDevice->getDevice(), mVkDeviceMemory);
}

void GPUBuffer::copy(const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, const GPUCommandPool& commandPool, const GPUDevice& vulkanDevice)
{   
    PROFILER_CPU_NAMED(buffer_copy)
    CHECK_MSG(sourceBuffer.mGPUBufferData.Size <= destinationBuffer.mGPUBufferData.Size, "sourceBuffer size <= destinationBuffer size: " + std::to_string(sourceBuffer.mGPUBufferData.Size) +" "+ std::to_string(destinationBuffer.mGPUBufferData.Size));

    constexpr u32 commandBufferCount = 1;
    const std::vector<GPUCommandBuffer> commandBuffers = commandPool.allocateCommandBuffers(commandBufferCount);
    CHECK_MSG(commandBuffers.size() == commandBufferCount, "commandBuffers.size() == commandBufferCount")

    const GPUCommandBuffer& commandBuffer = commandBuffers[0];
    VkCommandBuffer vkCommandBuffer = commandBuffer.getVkCommandBuffer();
    
    commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    {
        PROFILER_GPU_NAMED(copy_buffer, sourceBuffer.mGPUContext->mTracyContext, commandBuffer.getVkCommandBuffer());

        VkBufferCopy copyRegion{};
        copyRegion.size = sourceBuffer.mGPUBufferData.Size;
        constexpr u32 regionCount = 1;
        vkCmdCopyBuffer(vkCommandBuffer, sourceBuffer.mVkBuffer, destinationBuffer.mVkBuffer, regionCount, &copyRegion);
    }

    commandBuffer.end();

    {   
        PROFILER_CPU_NAMED(submit_copy_buffer)
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;

        constexpr u32 submitCount = 1;
        VkFence fence = VK_NULL_HANDLE;
        vkQueueSubmit(vulkanDevice.getGraphicsQueue(), submitCount, &submitInfo, fence);
        vkQueueWaitIdle(vulkanDevice.getGraphicsQueue());
    }

    commandPool.freeCommandBuffer(commandBuffer);
}