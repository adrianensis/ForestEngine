#pragma once

#include "GPU/Core/GPUContext.hpp"

class ImageData;
class GPUImageUtils
{
public:
    static VkImageView createImageView(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    static bool transitionImageLayout(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    static void copyBufferToImage(Ptr<GPUContext> gpuContext, VkBuffer buffer, VkImage image, u32 width, u32 height, i32 offsetX, i32 offsetY);
    static bool generateMipmaps(Ptr<GPUContext> gpuContext, const ImageData& imageData, VkImage image, VkFormat imageFormat, u32 mipMapLevels);
};
