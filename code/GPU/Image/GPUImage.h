#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUContext.hpp"

#include <vulkan/vulkan.h>

class GPUImageData
{
public:
    GPU::u32 Width = 0;
    GPU::u32 Height = 0;
    GPU::u32 MipLevels = 0;
    VkFormat Format = VK_FORMAT_UNDEFINED;
    VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags Usage = 0;
    VkMemoryPropertyFlags MemoryProperties = 0;
    VkImageLayout InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
    GPU::i32 mOffsetX = 0;
    GPU::i32 mOffsetY = 0;
    GPU::u32 mChannels = 0;
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
    VmaAllocation mAllocation = VK_NULL_HANDLE;
    GPUImageData mGPUImageData;
    VkImageLayout mCurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

public:
    auto getVkImage() const { return mVkImage; }
    auto getVkImageView() const { return mVkImageView; }
    const auto& getGPUImageData() const { return mGPUImageData; }
    auto getCurrentLayout() const { return mCurrentLayout; }
};