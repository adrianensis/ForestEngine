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