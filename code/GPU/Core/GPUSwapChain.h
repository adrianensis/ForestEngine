#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUPhysicalDevice.h"

#include <vulkan/vulkan.h>
#include <vector>

#include "Engine/Core.hpp"

class GPUSwapChain 
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPUSwapChain(GPUDevice* vulkanDevice, VkSurfaceKHR vkSurface);
    bool init(Maths::Vector2 windowSizeInPixels);
    void terminate();

private:
    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    Core::u32 getImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    bool createSwapChain(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, Core::u32 imageCount);
    bool findSwapChainImages(Core::u32 imageCount);
    bool createSwapChainImageViews();
    std::string getPresentationModeAsString(VkPresentModeKHR presentMode) const;

private:
    GPUDevice* vulkanDevice;
    Maths::Vector2 mWindowSizeInPixels;
    VkSurfaceKHR vkSurface;
    VkSurfaceFormatKHR mSurfaceFormat{};
    VkPresentModeKHR presentMode{};
    VkExtent2D mExtent{};
    VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;
public:
    CRGET(SwapChain);
    CRGET(SurfaceFormat);
    CRGET(Extent);
    CRGET(ImageViews);
    CRGET(Images);
};


