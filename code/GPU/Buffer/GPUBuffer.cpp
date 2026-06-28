#include "GPU/Buffer/GPUBuffer.h"
#include "GPU/Core/GPUCommandBuffer.h"
#include "GPU/Core/GPULog.h"
#include "GPU/Core/GPUMemoryAllocator.hpp"

bool GPUBuffer::init(GPUContext* gpuContext, const GPUBufferData& gpuBufferData)
{
    PROFILER_CPU_NAMED(buffer_init)
    mGPUContext = gpuContext;
    mGPUBufferData = gpuBufferData;

    // VkAllocationCallbacks* allocator = VK_NULL_HANDLE;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = mGPUBufferData.Size;
    bufferInfo.usage = mGPUBufferData.Usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // if (vkCreateBuffer(mGPUContext->vulkanDevice->getDevice(), &bufferInfo, allocator, &mVkBuffer) != VK_SUCCESS) {
    //     CHECK_MSG(false,"Could not create Vulkan buffer");
    //     return false;
    // }

    // VkMemoryRequirements memoryRequirements;
    // vkGetBufferMemoryRequirements(mGPUContext->vulkanDevice->getDevice(), mVkBuffer, &memoryRequirements);

    // VkMemoryAllocateInfo memoryAllocateInfo{};
    // memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    // memoryAllocateInfo.allocationSize = memoryRequirements.size;
    // memoryAllocateInfo.memoryTypeIndex = mGPUContext->vulkanPhysicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, mGPUBufferData.MemoryProperties);

    // if (vkAllocateMemory(mGPUContext->vulkanDevice->getDevice(), &memoryAllocateInfo, allocator, &mVkDeviceMemory) != VK_SUCCESS) {
    //     CHECK_MSG(false,"Could not allocate Vulkan vkBuffer memory");
    //     return false;
    // }
    
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = mGPUBufferData.MemoryUsage;
    
    
    VkResult result = vmaCreateBuffer(mGPUContext->mVmaAllocator, &bufferInfo, &allocInfo, &mVkBuffer, &mAllocation, nullptr);
    
    if(result != VK_SUCCESS)
    {
        // CHECK_MSG(false,"Could not create Vulkan buffer");
        return false;
    }
    // constexpr VkDeviceSize memoryOffset = 0;
    // vkBindBufferMemory(mGPUContext->vulkanDevice->getDevice(), mVkBuffer, mVkDeviceMemory, memoryOffset);
    
    mInit = true;
    // GPU_LOG("Initialized Vulkan buffer");
    return true;
}

void GPUBuffer::terminate()
{
    PROFILER_CPU_NAMED(buffer_terminate)
    if(mInit)
    {
        mGPUContext->vulkanDevice->waitUntilIdle();

        // VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
        // vkDestroyBuffer(mGPUContext->vulkanDevice->getDevice(), mVkBuffer, allocator);
        // GPU_LOG("Destroyed Vulkan buffer");
        // vkFreeMemory(mGPUContext->vulkanDevice->getDevice(), mVkDeviceMemory, allocator);
        // GPU_LOG("Freed Vulkan buffer memory");
        // GPU_LOG("Terminated Vulkan buffer");
        vmaDestroyBuffer(mGPUContext->mVmaAllocator, mVkBuffer, mAllocation);
        mAllocation = VK_NULL_HANDLE;
        mVkBuffer = VK_NULL_HANDLE;
        mInit = false;
    }
}

// void GPUBuffer::resize(GPU::u32 size)
// {
//     terminate();

//     GPUBufferData gpuBufferData = mGPUBufferData;
//     gpuBufferData.Size = size;
//     if(!init(mGPUContext, gpuBufferData))
//     {
//         CHECK_MSG(false,"Could not resize vertex buffer");
//     }
// }

void GPUBuffer::setData(const void* data, GPU::u32 size)
{
    PROFILER_CPU_NAMED(buffer_set_data)
    CHECK_MSG(size > 0, "size > 0")
    CHECK_MSG(size <= mGPUBufferData.Size, "size <= mGPUBufferData.Size")

    // void* memory = nullptr;
    // constexpr VkDeviceSize memoryOffset = 0;
    // constexpr VkMemoryMapFlags memoryMapFlags = 0;
    // vkMapMemory(mGPUContext->vulkanDevice->getDevice(), mVkDeviceMemory, memoryOffset, size, memoryMapFlags, &memory);
    // std::memcpy(memory, data, size);
    // vkUnmapMemory(mGPUContext->vulkanDevice->getDevice(), mVkDeviceMemory);

    void* mappedData = nullptr;
    vmaMapMemory(mGPUContext->mVmaAllocator, mAllocation, &mappedData);
    std::memcpy(mappedData, data, size);
    vmaUnmapMemory(mGPUContext->mVmaAllocator, mAllocation);
}

void GPUBuffer::copy(GPUContext* gpuContext, const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, VkCommandBuffer* vkCommandBuffer)
{   
    PROFILER_CPU_NAMED(buffer_copy)
    CHECK_MSG(sourceBuffer.mGPUBufferData.Size <= destinationBuffer.mGPUBufferData.Size, "sourceBuffer size <= destinationBuffer size: " + std::to_string(sourceBuffer.mGPUBufferData.Size) +" "+ std::to_string(destinationBuffer.mGPUBufferData.Size));

    VkCommandBuffer vkCommandBufferLocal;
    if(vkCommandBuffer)
    {
        vkCommandBufferLocal = *vkCommandBuffer;
    }
    else
    {
        vkCommandBufferLocal = gpuContext->beginSingleTimeCommands();
    }
    
    {
        PROFILER_GPU_NAMED(copy_buffer, sourceBuffer.mGPUContext->mTracyContext, vkCommandBufferLocal);

        VkBufferCopy copyRegion{};
        copyRegion.size = sourceBuffer.mGPUBufferData.Size;
        constexpr GPU::u32 regionCount = 1;
        vkCmdCopyBuffer(vkCommandBufferLocal, sourceBuffer.mVkBuffer, destinationBuffer.mVkBuffer, regionCount, &copyRegion);
    }

    if(!vkCommandBuffer)
    {
        gpuContext->endSingleTimeCommands(vkCommandBufferLocal, VK_NULL_HANDLE);
    }
}