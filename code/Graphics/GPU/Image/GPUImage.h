#pragma once

#include "Graphics/GPU/Core/GPUDevice.h"
#include "Graphics/GPU/Core/GPUContext.hpp"

#include <vulkan/vulkan.h>

class GPUImageData
{
public:
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

class GPUImage
{
public:
    bool init(Ptr<GPUContext> gpuContext, const GPUImageData& gpuImageData);
    void terminate();

private:
    Ptr<GPUContext> mGPUContext;
    VkImage mVkImage = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
public:
    CRGET(VkImage)
};