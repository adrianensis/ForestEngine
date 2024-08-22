#pragma once

#include "Core/ECS/System.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

#include "Graphics/GPU/GPUContext.hpp"
#include "Graphics/GPU/GPUImage.h"

class GPUInstance: public System
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
private:
    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxUniformBufferBindingPointsUniform = 0;
    i32 mMaxUniformBufferBindingPointsStorage = 0;


    struct Config {
        std::string Name;
        // Log::Level LogLevel;
        // GPUWindow::Config mWindow;
        Vulkan::Config mVulkan;
    };

public:
    OwnerPtr<GPUContext> mGPUContext;

public:
    RGET(GPUUniformBuffersContainer)
    CRGET(GPUUniformBuffersContainer)
};
REGISTER_CLASS(GPUInstance)
