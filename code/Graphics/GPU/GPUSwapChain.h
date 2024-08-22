#pragma once

#include "Graphics/GPU/GPUDevice.h"
#include "Graphics/GPU/GPUPhysicalDevice.h"
#include "Graphics/GPU/GPUWindow.hpp"

#include <vulkan/vulkan.h>
#include <vector>

#include "Core/Minimal.hpp"

class GPUSwapChain {
private:
    static const VkAllocationCallbacks* ALLOCATOR;

private:
    GPUDevice* vulkanDevice;
    GPUPhysicalDevice* vulkanPhysicalDevice;
    Vulkan* vulkan;
    GPUWindow* gpuWindow;
    VkSurfaceFormatKHR surfaceFormat{};
    VkPresentModeKHR presentMode{};
    VkExtent2D extent{};
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

public:
    GPUSwapChain(GPUDevice* vulkanDevice, GPUPhysicalDevice* vulkanPhysicalDevice, Vulkan* vulkan, GPUWindow* gpuWindow);

    const VkSwapchainKHR getSwapChain() const;

    const VkSurfaceFormatKHR& getSurfaceFormat() const;

    const VkExtent2D& getExtent() const;

    const std::vector<VkImageView>& getImageViews() const;

    bool initialize();

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
};


