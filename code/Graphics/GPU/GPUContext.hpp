#pragma once
#include "Graphics/GPU/Vulkan.h"
#include "Graphics/GPU/GPUPhysicalDevice.h"
#include "Graphics/GPU/GPUDevice.h"
#include "Graphics/GPU/GPUSwapChain.h"
#include "Graphics/GPU/GPUCommandPool.h"
#include "Graphics/GPU/GPUCommandBuffer.h"

class GPUContext
{
public:
    void init();

    inline static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    Vulkan* vulkan;
    GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUDevice* vulkanDevice;
    GPUSwapChain* vulkanSwapChain;
    GPUCommandPool* vulkanCommandPool;
    std::vector<GPUCommandBuffer*> vulkanCommandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;
};