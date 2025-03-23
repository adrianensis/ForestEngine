#include "GPU/Image/GPUImageUtils.hpp"
#include "Core/Image/ImageUtils.hpp"
#include "GPU/Buffer/GPUBuffer.h"

bool GPUImageUtils::transitionImageLayout(Ptr<GPUContext> gpuContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mipLevels)
{
    VkCommandBuffer commandBuffer = gpuContext->beginSingleTimeCommands();
    
    {
        PROFILER_GPU_NAMED(transitionImageLayout, gpuContext->mTracyContext, commandBuffer);

        VkPipelineStageFlags sourceStage = 0;
        VkPipelineStageFlags destinationStage = 0;
    
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext = nullptr;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
        imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
        imageMemoryBarrier.subresourceRange.baseArrayLayer= 0;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (hasStencilComponent(format))
            {
                imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
    
        const VkPipelineStageFlags all_shader_stages =
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
            VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    
        // source stage/access/layout
        switch (oldLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            {
                sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
                imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            break;
    
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            {
                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            {
                sourceStage = all_shader_stages;
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_GENERAL:
            {
                sourceStage = all_shader_stages;
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            {
                sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            {
                sourceStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            break;
    
            // case tr_texture_usage_resolve_src: { // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            //     sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            //     imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            //     imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            // }
            // break;
    
            // case tr_texture_usage_resolve_dst: { // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            //     sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            //     imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            //     imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            // }
            // break;
    
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            {
                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                imageMemoryBarrier.srcAccessMask = 0;
                imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
            break;

            default:
                CHECK_MSG(false,"Could not transition image layout: Unsupported transition");
        }
    
        // destination stage/access/layout
        switch (newLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            {
                destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
                imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            break;
    
           case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
           {
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            {
                destinationStage = all_shader_stages;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_GENERAL:
            {
                destinationStage = all_shader_stages;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            {
                destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
            break;
    
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            {
                destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            break;
    
            // case tr_texture_usage_resolve_src: { // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            //     destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            //     imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            //     imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            // }
            // break;
    
            // case tr_texture_usage_resolve_dst: { // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            //     destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            //     imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            //     imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            // }
            // break;
    
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            {
                destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                imageMemoryBarrier.dstAccessMask = 0;
                imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
            break;

            default:
                CHECK_MSG(false,"Could not transition image layout: Unsupported transition");
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
    gpuContext->endSingleTimeCommands(commandBuffer, VK_NULL_HANDLE);
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
    VkCommandBuffer commandBuffer = gpuContext->beginSingleTimeCommands();
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
    gpuContext->endSingleTimeCommands(commandBuffer, VK_NULL_HANDLE);
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

    VkCommandBuffer commandBuffer = gpuContext->beginSingleTimeCommands();
    {
        PROFILER_GPU_NAMED(generateMipmaps, gpuContext->mTracyContext, commandBuffer);

        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imageMemoryBarrier.subresourceRange.layerCount = 1;
        imageMemoryBarrier.subresourceRange.levelCount = 1;

        i32 mipWidth = width;
        i32 mipHeight = height;

        constexpr VkDependencyFlags dependencyFlags = 0;
        constexpr u32 memoryBarrierCount = 0;
        constexpr VkMemoryBarrier* memoryBarriers = VK_NULL_HANDLE;
        constexpr u32 bufferMemoryBarrierCount = 0;
        constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = VK_NULL_HANDLE;
        constexpr u32 imageMemoryBarrierCount = 1;

        for (u32 i = 1; i < mipMapLevels; i++) {
            imageMemoryBarrier.subresourceRange.baseMipLevel = i - 1;
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

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
                    &imageMemoryBarrier
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

            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

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
                    &imageMemoryBarrier
            );

            if (mipWidth > 1) {
                mipWidth /= 2;
            }
            if (mipHeight > 1) {
                mipHeight /= 2;
            }
        }

        imageMemoryBarrier.subresourceRange.baseMipLevel = mipMapLevels - 1;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

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
                &imageMemoryBarrier
        );
    }
    gpuContext->endSingleTimeCommands(commandBuffer, VK_NULL_HANDLE);
    return true;
}

void GPUImageUtils::copyImageToImage(Ptr<GPUContext> gpuContext, VkImage sourceImage, VkImageLayout sourceLayout, VkImage destinationImage, VkImageLayout destinationLayout, u32 width, u32 height, i32 offsetX, i32 offsetY)
    // VkCommandBuffer commandBuffer,
    // VkImage sourceImage,
    // VkExtent2D sourceExtent,
    // VkImage destinationImage,
    // VkExtent2D destinationExtent
// )
{
    // TODO: Ensure that the format and dimensions of the source and destination images are compatible for a direct copy

    VkCommandBuffer commandBuffer = gpuContext->beginSingleTimeCommands();
    {
        PROFILER_GPU_NAMED(generateMipmaps, gpuContext->mTracyContext, commandBuffer);

        // TODO: handle VK_FORMAT_UNDEFINED format, pass the correct format
        // TODO: handle mipsLevel == 1, pass the correct count
        GPUImageUtils::transitionImageLayout(gpuContext, sourceImage, VK_FORMAT_UNDEFINED, sourceLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1);

        // TODO: handle VK_FORMAT_UNDEFINED format, pass the correct format
        // TODO: handle mipsLevel == 1, pass the correct count
        GPUImageUtils::transitionImageLayout(gpuContext, destinationImage, VK_FORMAT_UNDEFINED, destinationLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);

        VkImageCopy copyRegion{};
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.srcSubresource.mipLevel = 0;
        copyRegion.srcSubresource.baseArrayLayer = 0;
        copyRegion.srcSubresource.layerCount = 1;
        copyRegion.srcOffset = {0, 0, 0}; // Top-left corner of the source image

        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.dstSubresource.mipLevel = 0;
        copyRegion.dstSubresource.baseArrayLayer = 0;
        copyRegion.dstSubresource.layerCount = 1;
        copyRegion.dstOffset = {0, 0, 0}; // Top-left corner of the destination image

        // The extent defines the size of the region to copy.
        // It should ideally match the dimensions of the source image
        // or the region you intend to copy.
        // copyRegion.extent.width = std::min(sourceExtent.width, destinationExtent.width);
        // copyRegion.extent.height = std::min(sourceExtent.height, destinationExtent.height);
        copyRegion.extent.width = width;
        copyRegion.extent.height = height;
        copyRegion.extent.depth = 1;

        vkCmdCopyImage(
            commandBuffer,
            sourceImage,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            destinationImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &copyRegion
        );

        // TODO: handle VK_FORMAT_UNDEFINED format, pass the correct format
        // TODO: handle mipsLevel == 1, pass the correct count
        GPUImageUtils::transitionImageLayout(gpuContext, destinationImage, VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, destinationLayout, 1);

    }
    gpuContext->endSingleTimeCommands(commandBuffer, VK_NULL_HANDLE);
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

    stagingBuffer.setData(data, imageSize);

    /*
        * Copy image texels from staging buffer to image
        */

    if (!GPUImageUtils::transitionImageLayout(gpuContext, textureImage, textureImageData.Format, textureImageData.InitialLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureImageData.MipLevels)) {
        CHECK_MSG(false,"Could not transition image layout from undefined to transfer destination");
        return false;
    }
    GPUImageUtils::copyBufferToImage(gpuContext, stagingBuffer.getVkBuffer(), textureImage, textureImageData.Width, textureImageData.Height, textureImageData.mOffsetX, textureImageData.mOffsetY);
    stagingBuffer.terminate();

    if (!GPUImageUtils::generateMipmaps(gpuContext, textureImageData.Width, textureImageData.Height, textureImage, textureImageData.Format, textureImageData.MipLevels)) {
        CHECK_MSG(false,"Could not generate mipmaps for texture image");
        return false;
    }

    // LOG("Initialized texture image");
    return true;
}

bool GPUImageUtils::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat GPUImageUtils::findDepthFormat(Ptr<GPUContext> gpuContext)
{
    std::vector<VkFormat> candidates = {
            VK_FORMAT_D32_SFLOAT_S8_UINT, // max priority for depth 32 bits stencil 8 bits 
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT
    };
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    return gpuContext->vulkanPhysicalDevice->findSupportedFormat(candidates, tiling, features);
}
