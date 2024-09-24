#include "GPUSwapChain.h"
#include <limits>


GPUSwapChain::GPUSwapChain(GPUDevice* vulkanDevice, VkSurfaceKHR vkSurface, Vector2 windowSizeInPixels)
        : vulkanDevice(vulkanDevice), vkSurface(vkSurface), windowSizeInPixels(windowSizeInPixels) {}

bool GPUSwapChain::init()
{
    const SwapChainInfo& swapChainInfo = vulkanDevice->getPhysicalDevice()->getSwapChainInfo();

    mSurfaceFormat = chooseSurfaceFormat(swapChainInfo.SurfaceFormats);
    presentMode = choosePresentMode(swapChainInfo.PresentModes);
    mExtent = chooseExtent(swapChainInfo.SurfaceCapabilities);

    uint32_t imageCount = getImageCount(swapChainInfo.SurfaceCapabilities);

    if (!createSwapChain(swapChainInfo.SurfaceCapabilities, imageCount))
    {
        CHECK_MSG(false,"Could not create Vulkan swap chain");
        return false;
    }
    VULKAN_LOG("Created Vulkan swap chain");

    if (!findSwapChainImages(imageCount))
    {
        CHECK_MSG(false,"Could not find any Vulkan swap chain images");
        return false;
    }
    VULKAN_LOG("Initialized [{}] Vulkan swap chain images");

    if (!createSwapChainImageViews())
    {
        CHECK_MSG(false,"Could not create Vulkan swap chain image views");
        return false;
    }
    VULKAN_LOG("Created [{}] Vulkan swap chain image views");

    return true;
}

void GPUSwapChain::terminate()
{
    for (VkImageView imageView : mImageViews)
    {
        vkDestroyImageView(vulkanDevice->getDevice(), imageView, ALLOCATOR);
    }
    mImageViews.clear();
    VULKAN_LOG("Destroyed Vulkan swap chain image views");
    vkDestroySwapchainKHR(vulkanDevice->getDevice(), mSwapChain, ALLOCATOR);
    VULKAN_LOG("Destroyed Vulkan swap chain");
}

VkSurfaceFormatKHR GPUSwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
{
    for (const auto& availableFormat: availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    VULKAN_LOG_WARNING("Could not find target format so defaulting to first available");
    return availableFormats[0];
}

VkPresentModeKHR GPUSwapChain::choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
{
    VkPresentModeKHR targetPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    for (const auto& availablePresentMode: availablePresentModes)
    {
        if (availablePresentMode == targetPresentMode)
        {
            return availablePresentMode;
        }
    }
    VkPresentModeKHR defaultPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    VULKAN_LOG_WARNING("Could not find "s + getPresentationModeAsString(targetPresentMode) + " present mode so defaulting to "s + getPresentationModeAsString(defaultPresentMode));
    return defaultPresentMode;
}

VkExtent2D GPUSwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const
{
    bool extentSizeCanDifferFromWindowResolution = surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max();
    if (!extentSizeCanDifferFromWindowResolution)
    {
        VULKAN_LOG("Extent should match gpuWindow resolution so using the surface capabilities extent");
        return surfaceCapabilities.currentExtent;
    }
    VULKAN_LOG("Extent can differ from gpuWindow resolution so picking the resolution that best matches the gpuWindow within the minImageExtent and maxImageExtent bounds");
    VkExtent2D extent =
    {
            (uint32_t) windowSizeInPixels.x,
            (uint32_t) windowSizeInPixels.y
    };
    extent.width = std::clamp(extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
    return extent;
}

uint32_t GPUSwapChain::getImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const
{
    uint32_t minImageCount = surfaceCapabilities.minImageCount;
    uint32_t maxImageCount = surfaceCapabilities.maxImageCount;
    uint32_t imageCount = minImageCount + 1;
    if (maxImageCount > 0 && imageCount > maxImageCount)
    {
        imageCount = maxImageCount;
    }
    return imageCount;
}

bool GPUSwapChain::createSwapChain(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32_t imageCount)
{
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = mSurfaceFormat.format;
    createInfo.imageColorSpace = mSurfaceFormat.colorSpace;
    createInfo.imageExtent = mExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const QueueFamilyIndices& queueFamilyIndices = vulkanDevice->getPhysicalDevice()->getQueueFamilyIndices();
    if (queueFamilyIndices.GraphicsFamily != queueFamilyIndices.PresentationFamily)
    {
        uint32_t queueFamilyIndexValues[] = {
                queueFamilyIndices.GraphicsFamily.value(),
                queueFamilyIndices.PresentationFamily.value()
        };
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.pQueueFamilyIndices = queueFamilyIndexValues;
        createInfo.queueFamilyIndexCount = 2;
    } 
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.pQueueFamilyIndices = nullptr;
        createInfo.queueFamilyIndexCount = 0;
    }

    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    return vkCreateSwapchainKHR(vulkanDevice->getDevice(), &createInfo, ALLOCATOR, &mSwapChain) == VK_SUCCESS;
}

bool GPUSwapChain::findSwapChainImages(uint32_t imageCount)
{
    vkGetSwapchainImagesKHR(vulkanDevice->getDevice(), mSwapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(vulkanDevice->getDevice(), mSwapChain, &imageCount, images.data());
    return !images.empty();
}

bool GPUSwapChain::createSwapChainImageViews()
{
    mImageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = mSurfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(vulkanDevice->getDevice(), &createInfo, ALLOCATOR, &mImageViews[i]) != VK_SUCCESS)
        {
            return false;
        }
    }
    return !mImageViews.empty();
}

std::string GPUSwapChain::getPresentationModeAsString(VkPresentModeKHR presentMode) const
{
    switch (presentMode)
    {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return "VK_PRESENT_MODE_IMMEDIATE_KHR";
        case VK_PRESENT_MODE_MAILBOX_KHR:
            return "VK_PRESENT_MODE_MAILBOX_KHR";
        case VK_PRESENT_MODE_FIFO_KHR:
            return "VK_PRESENT_MODE_FIFO_KHR";
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            return "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
            return "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            return "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
        case VK_PRESENT_MODE_MAX_ENUM_KHR:
            return "VK_PRESENT_MODE_MAX_ENUM_KHR";
        default:
            return "";
    }
}