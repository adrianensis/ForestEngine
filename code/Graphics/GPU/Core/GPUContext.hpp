#pragma once
#include "Graphics/GPU/Core/Vulkan.h"
#include "Graphics/GPU/Core/GPUPhysicalDevice.h"
#include "Graphics/GPU/Core/GPUDevice.h"
#include "Graphics/GPU/Core/GPUSwapChain.h"
#include "Graphics/GPU/Core/GPUCommandPool.h"
#include "Graphics/GPU/Core/GPUCommandBuffer.h"

class GPUContext
{
public:
    void init();
    void terminate();
private:
    bool createSurface();
    void destroySurface() const;

public:
    inline static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    Vulkan* vulkan;
    GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUDevice* vulkanDevice;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    GPUSwapChain* vulkanSwapChain;
    GPUCommandPool* vulkanCommandPool;
    std::vector<GPUCommandBuffer*> vulkanCommandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
};