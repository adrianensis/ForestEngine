#include "GPUImage.h"
#include "GPU/Image/GPUImageUtils.hpp"

bool GPUImage::init(Core::Ptr<GPUContext> gpuContext, const GPUImageData& gpuImageData)
{
    mGPUContext = gpuContext;
    mGPUImageData = gpuImageData;
    mCurrentLayout = mGPUImageData.InitialLayout;
    constexpr VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = mGPUImageData.Width;
    imageInfo.extent.height = mGPUImageData.Height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mGPUImageData.MipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = mGPUImageData.Format;
    imageInfo.tiling = mGPUImageData.Tiling;
    imageInfo.initialLayout = mGPUImageData.InitialLayout;
    imageInfo.usage = mGPUImageData.Usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = mGPUImageData.SampleCount;

    if (vkCreateImage(mGPUContext->vulkanDevice->getDevice(), &imageInfo, allocationCallbacks, &mVkImage) != VK_SUCCESS)
    {
        CHECK_MSG(false,"Could not create Vulkan image from texture");
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(mGPUContext->vulkanDevice->getDevice(), mVkImage, &memoryRequirements);

    Core::u32 memoryTypeIndex = mGPUContext->vulkanPhysicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, mGPUImageData.MemoryProperties);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(mGPUContext->vulkanDevice->getDevice(), &memoryAllocateInfo, allocationCallbacks, &vkDeviceMemory) != VK_SUCCESS)
    {
        CHECK_MSG(false,"Could not allocate Vulkan image memory");
        return false;
    }

    constexpr Core::u32 memoryOffset = 0;
    vkBindImageMemory(mGPUContext->vulkanDevice->getDevice(), mVkImage, vkDeviceMemory, memoryOffset);

    // LOG("Initialized image");
    return true;
}

void GPUImage::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImage(mGPUContext->vulkanDevice->getDevice(), mVkImage, allocationCallbacks);
    vkFreeMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory, allocationCallbacks);
    LOG("Terminated image");
}

void GPUImage::transition(VkImageLayout destinationLayout)
{
    GPUImageUtils::transitionImageLayout(mGPUContext, mVkImage, mGPUImageData.Format, mCurrentLayout, destinationLayout, mGPUImageData.MipLevels);
    mCurrentLayout = destinationLayout;
}

void GPUImage::copyToImage(GPUImage& destinationImage)
{
    copyToVkImage(destinationImage.getVkImage(), destinationImage.getCurrentLayout());
}

void GPUImage::copyToVkImage(VkImage destinationImage, VkImageLayout destinationLayout)
{
    GPUImageUtils::copyImageToImage(mGPUContext, mVkImage, mCurrentLayout, destinationImage, destinationLayout, mGPUImageData.Width, mGPUImageData.Height,0,0, mGPUImageData.MipLevels);
}