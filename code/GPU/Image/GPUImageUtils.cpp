#include "GPU/Image/GPUImageUtils.hpp"
#include "GPU/GPUUtils.hpp"
#include "Core/Image/ImageUtils.hpp"
#include "GPU/Buffer/GPUBuffer.h"

bool GPUImageUtils::transitionImageLayout(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mipLevels)
{
    VkCommandBuffer commandBuffer = GPUUtils::beginSingleTimeCommands(gpuContext);
    
    {
        PROFILER_GPU_NAMED(transitionImageLayout, gpuContext->mTracyContext, commandBuffer);

        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout = oldLayout;
        imageMemoryBarrier.newLayout = newLayout;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = 0;
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
        imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (GPUUtils::hasStencilComponent(gpuContext, format)) {
                imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            CHECK_MSG(false,"Could not transition image layout: Unsupported transition");
            return false;
        }

        constexpr VkDependencyFlags dependencyFlags = 0;
        constexpr u32 memoryBarrierCount = 0;
        constexpr VkMemoryBarrier* memoryBarriers = VK_NULL_HANDLE;
        constexpr u32 bufferMemoryBarrierCount = 0;
        constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = VK_NULL_HANDLE;
        constexpr u32 imageMemoryBarrierCount = 1;
        vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage,
                destinationStage,
                dependencyFlags,
                memoryBarrierCount,
                memoryBarriers,
                bufferMemoryBarrierCount,
                bufferMemoryBarriers,
                imageMemoryBarrierCount,
                &imageMemoryBarrier
        );
    }
    GPUUtils::endSingleTimeCommands(gpuContext, commandBuffer);
    return true;
}

VkImageView GPUImageUtils::createImageView(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels) {
    
    PROFILER_CPU_NAMED(createImageView);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(gpuContext->vulkanDevice->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan image view");
        return nullptr;
    }
    return imageView;
}

void GPUImageUtils::copyBufferToImage(Ptr<GPUContext> gpuContext, VkBuffer buffer, VkImage image, u32 width, u32 height, i32 offsetX, i32 offsetY)
{
    VkCommandBuffer commandBuffer = GPUUtils::beginSingleTimeCommands(gpuContext);
    {
        PROFILER_GPU_NAMED(copyBufferToImage, gpuContext->mTracyContext, commandBuffer);

        VkBufferImageCopy bufferImageCopy{};
        bufferImageCopy.bufferOffset = 0;
        bufferImageCopy.bufferRowLength = 0;
        bufferImageCopy.bufferImageHeight = 0;

        bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferImageCopy.imageSubresource.mipLevel = 0;
        bufferImageCopy.imageSubresource.baseArrayLayer = 0;
        bufferImageCopy.imageSubresource.layerCount = 1;

        bufferImageCopy.imageOffset = {0, 0, 0};

        constexpr u32 depth = 1;
        bufferImageCopy.imageExtent = {width, height, depth };
        bufferImageCopy.imageOffset = {offsetX, offsetY, 0 };

        constexpr u32 regionCount = 1;
        constexpr VkImageLayout imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        vkCmdCopyBufferToImage(
                commandBuffer,
                buffer,
                image,
                imageLayout,
                regionCount,
                &bufferImageCopy
        );
    }
    GPUUtils::endSingleTimeCommands(gpuContext,commandBuffer);
}


bool GPUImageUtils::generateMipmaps(Ptr<GPUContext> gpuContext, u32 width, u32 height, VkImage image, VkFormat imageFormat, u32 mipMapLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(gpuContext->vulkanPhysicalDevice->getPhysicalDevice(), imageFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        CHECK_MSG(false,"Image format does not support linear blitting");
        return false;
    }

    VkCommandBuffer commandBuffer = GPUUtils::beginSingleTimeCommands(gpuContext);
    {
        PROFILER_GPU_NAMED(generateMipmaps, gpuContext->mTracyContext, commandBuffer);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        i32 mipWidth = width;
        i32 mipHeight = height;

        constexpr VkDependencyFlags dependencyFlags = 0;
        constexpr u32 memoryBarrierCount = 0;
        constexpr VkMemoryBarrier* memoryBarriers = VK_NULL_HANDLE;
        constexpr u32 bufferMemoryBarrierCount = 0;
        constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = VK_NULL_HANDLE;
        constexpr u32 imageMemoryBarrierCount = 1;

        for (u32 i = 1; i < mipMapLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            VkPipelineStageFlagBits sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            VkPipelineStageFlagBits destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            vkCmdPipelineBarrier(
                    commandBuffer,
                    sourceStage,
                    destinationStage,
                    dependencyFlags,
                    memoryBarrierCount,
                    memoryBarriers,
                    bufferMemoryBarrierCount,
                    bufferMemoryBarriers,
                    imageMemoryBarrierCount,
                    &barrier
            );

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            VkImageLayout srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            VkImageLayout dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            VkFilter filter = VK_FILTER_LINEAR;
            constexpr u32 regionCount = 1;
            vkCmdBlitImage(
                    commandBuffer,
                    image,
                    srcImageLayout,
                    image,
                    dstImageLayout,
                    regionCount,
                    &blit,
                    filter
            );

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            vkCmdPipelineBarrier(
                    commandBuffer,
                    sourceStage,
                    destinationStage,
                    dependencyFlags,
                    memoryBarrierCount,
                    memoryBarriers,
                    bufferMemoryBarrierCount,
                    bufferMemoryBarriers,
                    imageMemoryBarrierCount,
                    &barrier
            );

            if (mipWidth > 1) {
                mipWidth /= 2;
            }
            if (mipHeight > 1) {
                mipHeight /= 2;
            }
        }

        barrier.subresourceRange.baseMipLevel = mipMapLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        VkPipelineStageFlagBits sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkPipelineStageFlagBits destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage,
                destinationStage,
                dependencyFlags,
                memoryBarrierCount,
                memoryBarriers,
                bufferMemoryBarrierCount,
                bufferMemoryBarriers,
                imageMemoryBarrierCount,
                &barrier
        );
    }
    GPUUtils::endSingleTimeCommands(gpuContext, commandBuffer);
    return true;
}

bool GPUImageUtils::createTextureImage(Ptr<GPUContext> gpuContext, VkImage textureImage, const GPUImageData& textureImageData, byte* data) 
{
    PROFILER_CPU_NAMED(createTextureImage)

    /*
    * Copy image texels to staging buffer
    */

    VkDeviceSize imageSize = textureImageData.Width * textureImageData.Height * textureImageData.mChannels;//STBI_rgb_alpha;
    GPUBuffer stagingBuffer;

    GPUBufferData stagingBufferConfig{};
    stagingBufferConfig.Size = imageSize;
    stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    if (!stagingBuffer.init(gpuContext, stagingBufferConfig)) {
        CHECK_MSG(false,"Could not initialize texture image stagingBuffer");
        return false;
    }

    stagingBuffer.setData(data);

    /*
        * Copy image texels from staging buffer to image
        */

    if (!GPUImageUtils::transitionImageLayout(gpuContext, textureImage, textureImageData.Format, textureImageData.Layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureImageData.MipLevels)) {
        CHECK_MSG(false,"Could not transition image layout from undefined to transfer destination");
        return false;
    }
    GPUImageUtils::copyBufferToImage(gpuContext, stagingBuffer.getVkBuffer(), textureImage, textureImageData.Width, textureImageData.Height, textureImageData.mOffsetX, textureImageData.mOffsetY);
    stagingBuffer.terminate();

    if (!GPUImageUtils::generateMipmaps(gpuContext, textureImageData.Width, textureImageData.Height, textureImage, textureImageData.Format, textureImageData.MipLevels)) {
        CHECK_MSG(false,"Could not generate mipmaps for texture image");
        return false;
    }

    LOG("Initialized texture image");
    return true;
}