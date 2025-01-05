#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUContext.hpp"

#include <vulkan/vulkan.h>

class GPUImageData
{
public:
    u32 Width;
    u32 Height;
    u32 MipLevels;
    VkFormat Format;
    VkImageTiling Tiling;
    VkImageUsageFlags Usage;
    VkMemoryPropertyFlags MemoryProperties;
    VkImageLayout Layout;
    VkSampleCountFlagBits SampleCount;
    i32 mOffsetX = 0;
    i32 mOffsetY = 0;
    u32 mChannels = 0;
};

class GPUImage
{
public:
    bool init(WeakPtr<GPUContext> gpuContext, const GPUImageData& gpuImageData);
    void terminate();

private:
    WeakPtr<GPUContext> mGPUContext;
    VkImage mVkImage = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
public:
    CRGET(VkImage)
};