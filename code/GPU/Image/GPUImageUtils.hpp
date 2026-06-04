#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUImage.h"

class ImageData;
class GPUImageUtils
{
public:
    static bool createTextureImage(GPUContext* gpuContext, VkImage textureImage, const GPUImageData& textureImageData, GPU::byte* data);
    static VkImageView createImageView(GPUContext* gpuContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, GPU::u32 mipLevels);
    static bool transitionImageLayout(GPUContext* gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, GPU::u32 mipLevels);
    static void copyBufferToImage(GPUContext* gpuContext, VkBuffer buffer, VkImage image, GPU::u32 width, GPU::u32 height, GPU::i32 offsetX, GPU::i32 offsetY);
    static void copyImageToImage(GPUContext* gpuContext, VkImage sourceImage, VkImageLayout sourceLayout, VkImage destinationImage, VkImageLayout destinationLayout, GPU::u32 width, GPU::u32 height, GPU::i32 offsetX, GPU::i32 offsetY, GPU::u32 mipLevels);
    static bool generateMipmaps(GPUContext* gpuContext, GPU::u32 width, GPU::u32 height, VkImage image, VkFormat imageFormat, GPU::u32 mipMapLevels);
    static bool hasStencilComponent(VkFormat format);
    static VkFormat findDepthFormat(GPUContext* gpuContext);
};
