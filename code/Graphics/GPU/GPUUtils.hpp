#pragma once

#include "Graphics/GPU/Core/GPUContext.hpp"

class GPUContext;
class GPUUtils
{
public:
    static VkCommandBuffer beginSingleTimeCommands(Ptr<GPUContext> gpuContext);
    static void endSingleTimeCommands(Ptr<GPUContext> gpuContext, VkCommandBuffer commandBuffer);
    static VkImageView createImageView(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    static bool transitionImageLayout(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    static bool hasStencilComponent(Ptr<GPUContext> gpuContext, VkFormat format);
    static bool initializeSyncObjects(Ptr<GPUContext> gpuContext);
};
