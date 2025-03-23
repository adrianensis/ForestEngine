#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUImage.h"

class ImageData;
class GPUImageUtils
{
public:
    static bool createTextureImage(Ptr<GPUContext> gpuContext, VkImage textureImage, const GPUImageData& textureImageData, byte* data);
    static VkImageView createImageView(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);
    static bool transitionImageLayout(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mipLevels);
    static void copyBufferToImage(Ptr<GPUContext> gpuContext, VkBuffer buffer, VkImage image, u32 width, u32 height, i32 offsetX, i32 offsetY);
    static void copyImageToImage(Ptr<GPUContext> gpuContext, VkImage sourceImage, VkImageLayout sourceLayout, VkImage destinationImage, VkImageLayout destinationLayout, u32 width, u32 height, i32 offsetX, i32 offsetY, u32 mipLevels);
    static bool generateMipmaps(Ptr<GPUContext> gpuContext, u32 width, u32 height, VkImage image, VkFormat imageFormat, u32 mipMapLevels);
    static bool hasStencilComponent(VkFormat format);
    static VkFormat findDepthFormat(Ptr<GPUContext> gpuContext);
};
