#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUImage.h"

class ImageData;
class GPUImageUtils
{
public:
    static bool createTextureImage(WeakPtr<GPUContext> gpuContext, VkImage textureImage, const GPUImageData& textureImageData, byte* data);
    static VkImageView createImageView(WeakPtr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);
    static bool transitionImageLayout(WeakPtr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mipLevels);
    static void copyBufferToImage(WeakPtr<GPUContext> gpuContext, VkBuffer buffer, VkImage image, u32 width, u32 height, i32 offsetX, i32 offsetY);
    static bool generateMipmaps(WeakPtr<GPUContext> gpuContext, u32 width, u32 height, VkImage image, VkFormat imageFormat, u32 mipMapLevels);
    static bool hasStencilComponent(VkFormat format);
};
