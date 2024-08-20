#pragma once

#include "Core/ECS/System.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

#include "Graphics/GPU/Vulkan/Log.h"
#include "Graphics/GPU/Vulkan/Vulkan.h"
#include "Graphics/GPU/Vulkan/GPUPhysicalDevice.h"
#include "Graphics/GPU/Vulkan/GPUDevice.h"
#include "Graphics/GPU/Vulkan/GPUSwapChain.h"
#include "Graphics/GPU/Vulkan/GPUCommandPool.h"
#include "Graphics/GPU/Vulkan/GPUCommandBuffer.h"
#include "Graphics/GPU/Vulkan/GPUFramebuffer.h"
#include "Graphics/GPU/Vulkan/GPUImage.h"

class GPUGlobalState: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    u32 requestSharedBufferBindingPoint(GPUBufferType gpuSharedBufferType);
    VkCommandBuffer beginSingleTimeCommands() const;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    bool transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) const;
    bool hasStencilComponent(VkFormat format) const;

private:
    bool initializeSyncObjects();

public:
    inline static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
private:
    GPUSharedBuffersContainer mGPUSharedBuffersContainer;
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxSharedBufferBindingPointsUniform = 0;
    i32 mMaxSharedBufferBindingPointsStorage = 0;


    struct Config {
        std::string Name;
        GPUAPI::Log::Level LogLevel;
        // GPUAPI::Window::Config mWindow;
        GPUAPI::Vulkan::Config mVulkan;
    };

public:
    GPUAPI::Vulkan* vulkan;
    GPUAPI::GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUAPI::GPUDevice* vulkanDevice;
    GPUAPI::GPUSwapChain* vulkanSwapChain;
    GPUAPI::GPUCommandPool* vulkanCommandPool;
    std::vector<GPUAPI::GPUCommandBuffer> vulkanCommandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

public:
    RGET(GPUSharedBuffersContainer)
    CRGET(GPUSharedBuffersContainer)
};
REGISTER_CLASS(GPUGlobalState)
