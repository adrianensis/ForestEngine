#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Image/GPUImage.h"

class ImageData;
class GPUImageUtils
{
public:
    static bool createTextureImage(Core::Ptr<GPUContext> gpuContext, VkImage textureImage, const GPUImageData& textureImageData, Core::byte* data);
    static VkImageView createImageView(Core::Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, Core::u32 mipLevels);
    static bool transitionImageLayout(Core::Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Core::u32 mipLevels);
    static void copyBufferToImage(Core::Ptr<GPUContext> gpuContext, VkBuffer buffer, VkImage image, Core::u32 width, Core::u32 height, Core::i32 offsetX, Core::i32 offsetY);
    static void copyImageToImage(Core::Ptr<GPUContext> gpuContext, VkImage sourceImage, VkImageLayout sourceLayout, VkImage destinationImage, VkImageLayout destinationLayout, Core::u32 width, Core::u32 height, Core::i32 offsetX, Core::i32 offsetY, Core::u32 mipLevels);
    static bool generateMipmaps(Core::Ptr<GPUContext> gpuContext, Core::u32 width, Core::u32 height, VkImage image, VkFormat imageFormat, Core::u32 mipMapLevels);
    static bool hasStencilComponent(VkFormat format);
    static VkFormat findDepthFormat(Core::Ptr<GPUContext> gpuContext);
};
