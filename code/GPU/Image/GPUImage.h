#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUContext.hpp"

#include <vulkan/vulkan.h>

class GPUImageData
{
public:
    Core::u32 Width = 0;
    Core::u32 Height = 0;
    Core::u32 MipLevels = 0;
    VkFormat Format = VK_FORMAT_UNDEFINED;
    VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags Usage = 0;
    VkMemoryPropertyFlags MemoryProperties = 0;
    VkImageLayout InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
    Core::i32 mOffsetX = 0;
    Core::i32 mOffsetY = 0;
    Core::u32 mChannels = 0;
};

class GPUImage
{
public:
    bool init(GPUContext* gpuContext, const GPUImageData& gpuImageData);
    void transition(VkImageLayout destinationLayout);
    void copyToImage(GPUImage& destinationImage);
    void copyToVkImage(VkImage destinationImage, VkImageLayout destinationLayout);
    void terminate();

private:
    GPUContext* mGPUContext = nullptr;
    VkImage mVkImage = VK_NULL_HANDLE;
    VkImageView mVkImageView = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
    GPUImageData mGPUImageData;
    VkImageLayout mCurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
public:
    CRGET(VkImage)
    CRGET(VkImageView)
    CRGET(GPUImageData)
    GET(CurrentLayout)
};