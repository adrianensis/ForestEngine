#include "GPUImage.h"

bool GPUImage::init(Ptr<GPUContext> gpuContext, const GPUImageData& gpuImageData)
{
    mGPUContext = gpuContext;
    constexpr VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = gpuImageData.Width;
    imageInfo.extent.height = gpuImageData.Height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = gpuImageData.MipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = gpuImageData.Format;
    imageInfo.tiling = gpuImageData.Tiling;
    imageInfo.initialLayout = gpuImageData.Layout;
    imageInfo.usage = gpuImageData.Usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = gpuImageData.SampleCount;

    if (vkCreateImage(mGPUContext->vulkanDevice->getDevice(), &imageInfo, allocationCallbacks, &mVkImage) != VK_SUCCESS)
    {
        CHECK_MSG(false,"Could not create Vulkan image from texture");
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(mGPUContext->vulkanDevice->getDevice(), mVkImage, &memoryRequirements);

    uint32_t memoryTypeIndex = mGPUContext->vulkanPhysicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, gpuImageData.MemoryProperties);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(mGPUContext->vulkanDevice->getDevice(), &memoryAllocateInfo, allocationCallbacks, &vkDeviceMemory) != VK_SUCCESS)
    {
        CHECK_MSG(false,"Could not allocate Vulkan image memory");
        return false;
    }

    constexpr uint32_t memoryOffset = 0;
    vkBindImageMemory(mGPUContext->vulkanDevice->getDevice(), mVkImage, vkDeviceMemory, memoryOffset);

    LOG("Initialized image");
    return true;
}

void GPUImage::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImage(mGPUContext->vulkanDevice->getDevice(), mVkImage, allocationCallbacks);
    vkFreeMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory, allocationCallbacks);
    LOG("Terminated image");
}