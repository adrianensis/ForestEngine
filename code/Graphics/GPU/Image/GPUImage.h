#pragma once

#include "Graphics/GPU/Core/GPUDevice.h"
#include "Graphics/GPU/Core/GPUContext.hpp"

#include <vulkan/vulkan.h>

class GPUImage {
public:
    struct Config {
        uint32_t Width;
        uint32_t Height;
        uint32_t MipLevels;
        VkFormat Format;
        VkImageTiling Tiling;
        VkImageUsageFlags Usage;
        VkMemoryPropertyFlags MemoryProperties;
        VkImageLayout Layout;
        VkSampleCountFlagBits SampleCount;
    };

public:
    VkImage getVkImage() const;
    bool init(Ptr<GPUContext> gpuContext, const Config& config);
    void terminate();

private:
    Ptr<GPUContext> mGPUContext;
    VkImage vkImage = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
};