#pragma once

#include "Graphics/GPU/Core/GPUDevice.h"
#include "Graphics/GPU/Core/GPUPhysicalDevice.h"
#include "Graphics/GPU/GPUWindow.hpp"

#include <vulkan/vulkan.h>
#include <vector>

#include "Core/Minimal.hpp"

class GPUSwapChain 
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPUSwapChain(GPUDevice* vulkanDevice, VkSurfaceKHR vkSurface, Vector2 windowSizeInPixels);
    bool init();
    void terminate();

private:
    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    uint32_t getImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    bool createSwapChain(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32_t imageCount);
    bool findSwapChainImages(uint32_t imageCount);
    bool createSwapChainImageViews();
    std::string getPresentationModeAsString(VkPresentModeKHR presentMode) const;

private:
    GPUDevice* vulkanDevice;
    Vector2 windowSizeInPixels;
    VkSurfaceKHR vkSurface;
    VkSurfaceFormatKHR mSurfaceFormat{};
    VkPresentModeKHR presentMode{};
    VkExtent2D mExtent{};
    VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;
    std::vector<VkImage> images;
    std::vector<VkImageView> mImageViews;
public:
    CRGET(SwapChain);
    CRGET(SurfaceFormat);
    CRGET(Extent);
    CRGET(ImageViews);
};


