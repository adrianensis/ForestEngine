#include "GPUImage.h"

VkImage GPUImage::getVkImage() const {
    return vkImage;
}

bool GPUImage::init(Ptr<GPUContext> gpuContext, const Config& config)
{
    mGPUContext = gpuContext;
    constexpr VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = config.Width;
    imageInfo.extent.height = config.Height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = config.MipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = config.Format;
    imageInfo.tiling = config.Tiling;
    imageInfo.initialLayout = config.Layout;
    imageInfo.usage = config.Usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = config.SampleCount;

    if (vkCreateImage(mGPUContext->vulkanDevice->getDevice(), &imageInfo, allocationCallbacks, &vkImage) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan image from texture");
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(mGPUContext->vulkanDevice->getDevice(), vkImage, &memoryRequirements);

    uint32_t memoryTypeIndex = mGPUContext->vulkanPhysicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, config.MemoryProperties);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(mGPUContext->vulkanDevice->getDevice(), &memoryAllocateInfo, allocationCallbacks, &vkDeviceMemory) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not allocate Vulkan image memory");
        return false;
    }

    constexpr uint32_t memoryOffset = 0;
    vkBindImageMemory(mGPUContext->vulkanDevice->getDevice(), vkImage, vkDeviceMemory, memoryOffset);

    LOG("Initialized image");
    return true;
}

void GPUImage::terminate() {
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImage(mGPUContext->vulkanDevice->getDevice(), vkImage, allocationCallbacks);
    vkFreeMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory, allocationCallbacks);
    LOG("Terminated image");
}