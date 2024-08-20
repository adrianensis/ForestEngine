#pragma once

#include "Core/ECS/System.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

#include "Graphics/GPU/Vulkan.h"
#include "Graphics/GPU/GPUPhysicalDevice.h"
#include "Graphics/GPU/GPUDevice.h"
#include "Graphics/GPU/GPUSwapChain.h"
#include "Graphics/GPU/GPUCommandPool.h"
#include "Graphics/GPU/GPUCommandBuffer.h"
#include "Graphics/GPU/GPUImage.h"

class GPUGlobalState: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    u32 requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType);
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
    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxUniformBufferBindingPointsUniform = 0;
    i32 mMaxUniformBufferBindingPointsStorage = 0;


    struct Config {
        std::string Name;
        Log::Level LogLevel;
        // Window::Config mWindow;
        Vulkan::Config mVulkan;
    };

public:
    Vulkan* vulkan;
    GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUDevice* vulkanDevice;
    GPUSwapChain* vulkanSwapChain;
    GPUCommandPool* vulkanCommandPool;
    std::vector<GPUCommandBuffer> vulkanCommandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

public:
    RGET(GPUUniformBuffersContainer)
    CRGET(GPUUniformBuffersContainer)
};
REGISTER_CLASS(GPUGlobalState)
