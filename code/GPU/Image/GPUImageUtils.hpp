#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUImage.h"

class ImageData;
class GPUImageUtils
{
public:
    static bool createTextureImage(Core::Ptr<GPUContext> gpuContext, VkImage textureImage, const GPUImageData& textureImageData, byte* data);
    static VkImageView createImageView(Core::Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);
    static bool transitionImageLayout(Core::Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mipLevels);
    static void copyBufferToImage(Core::Ptr<GPUContext> gpuContext, VkBuffer buffer, VkImage image, u32 width, u32 height, i32 offsetX, i32 offsetY);
    static void copyImageToImage(Core::Ptr<GPUContext> gpuContext, VkImage sourceImage, VkImageLayout sourceLayout, VkImage destinationImage, VkImageLayout destinationLayout, u32 width, u32 height, i32 offsetX, i32 offsetY, u32 mipLevels);
    static bool generateMipmaps(Core::Ptr<GPUContext> gpuContext, u32 width, u32 height, VkImage image, VkFormat imageFormat, u32 mipMapLevels);
    static bool hasStencilComponent(VkFormat format);
    static VkFormat findDepthFormat(Core::Ptr<GPUContext> gpuContext);
};
