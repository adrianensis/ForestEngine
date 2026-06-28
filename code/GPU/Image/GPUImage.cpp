#include "GPUImage.h"
#include "GPU/Image/GPUImageUtils.hpp"
#include "vulkan/vulkan_core.h"

bool GPUImage::init(GPUContext* gpuContext, const GPUImageData& gpuImageData)
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

    // if (vkCreateImage(mGPUContext->vulkanDevice->getDevice(), &imageInfo, allocationCallbacks, &mVkImage) != VK_SUCCESS)
    // {
    //     CHECK_MSG(false,"Could not create Vulkan image from texture");
    //     return false;
    // }

    // VkMemoryRequirements memoryRequirements;
    // vkGetImageMemoryRequirements(mGPUContext->vulkanDevice->getDevice(), mVkImage, &memoryRequirements);

    // GPU::u32 memoryTypeIndex = mGPUContext->vulkanPhysicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, mGPUImageData.MemoryProperties);

    // VkMemoryAllocateInfo memoryAllocateInfo{};
    // memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    // memoryAllocateInfo.allocationSize = memoryRequirements.size;
    // memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    // if (vkAllocateMemory(mGPUContext->vulkanDevice->getDevice(), &memoryAllocateInfo, allocationCallbacks, &vkDeviceMemory) != VK_SUCCESS)
    // {
    //     CHECK_MSG(false,"Could not allocate Vulkan image memory");
    //     return false;
    // }

    // constexpr GPU::u32 memoryOffset = 0;
    // vkBindImageMemory(mGPUContext->vulkanDevice->getDevice(), mVkImage, vkDeviceMemory, memoryOffset);

    VmaAllocationCreateInfo allocInfo = {};
    // Map your GPUImageData.MemoryProperties to VMA usage flags
    // If you are using VMA_MEMORY_USAGE_AUTO, it handles this internally
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO; 

    // THE VMA REPLACEMENT:
    // This replaces vkCreateImage, vkGetImageMemoryRequirements, and vkAllocateMemory
    if (vmaCreateImage(mGPUContext->mVmaAllocator, &imageInfo, &allocInfo, &mVkImage, &mAllocation, nullptr) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create Vulkan image via VMA");
        return false;
    }

    // LOG("Initialized image");

    VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

    // Check if this image is intended for Depth/Stencil usage
    if (mGPUImageData.Usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) 
    {
        aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        
        // If the format has a stencil component, you might need the stencil bit too
        if (GPUImageUtils::hasStencilComponent(mGPUImageData.Format)) {
            aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    
    mVkImageView = GPUImageUtils::createImageView(mGPUContext, mVkImage, mGPUImageData.Format, aspectFlags, mGPUImageData.MipLevels);
    if (!mVkImageView)
    {
        CHECK_MSG(false,"Could not create Vulkan image view");
    }

    return true;
}

void GPUImage::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), mVkImageView, allocationCallbacks);
    // vkDestroyImage(mGPUContext->vulkanDevice->getDevice(), mVkImage, allocationCallbacks);
    // vkFreeMemory(mGPUContext->vulkanDevice->getDevice(), vkDeviceMemory, allocationCallbacks);
    if (mVkImage != VK_NULL_HANDLE) 
    {
        vmaDestroyImage(mGPUContext->mVmaAllocator, mVkImage, mAllocation);
        
        mVkImage = VK_NULL_HANDLE;
        mAllocation = VK_NULL_HANDLE;
    }
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