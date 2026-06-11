#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Window/GPUWindow.hpp"

#include <vulkan/vulkan.h>
#include <vector>

class GPUSwapChain 
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPUSwapChain(GPUDevice* vulkanDevice, VkSurfaceKHR vkSurface);
    bool init(GPUWindowSize windowSizeInPixels);
    void terminate();

private:
    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    GPU::u32 getImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    bool createSwapChain(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, GPU::u32 imageCount);
    bool findSwapChainImages(GPU::u32 imageCount);
    bool createSwapChainImageViews();
    std::string getPresentationModeAsString(VkPresentModeKHR presentMode) const;

private:
    GPUDevice* vulkanDevice = nullptr;
    GPUWindowSize mWindowSizeInPixels;
    VkSurfaceKHR vkSurface;
    VkSurfaceFormatKHR mSurfaceFormat{};
    VkPresentModeKHR presentMode{};
    VkExtent2D mExtent{};
    VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;
public:
    auto getSwapChain() const { return mSwapChain; };
    auto getSurfaceFormat() const { return mSurfaceFormat; };
    const auto& getExtent() const { return mExtent; };
    const auto& getImageViews() const { return mImageViews; };
    const auto& getImages() const { return mImages; };
};


