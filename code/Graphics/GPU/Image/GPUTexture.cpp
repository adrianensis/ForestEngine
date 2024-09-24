#include "Graphics/GPU/Image/GPUTexture.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/GPU/Buffer/GPUBuffer.h"
#include "Core/Image/ImageUtils.hpp"

void GPUTexture::enable(u32 textureUnit) const
{
//    GET_SYSTEM(GPUInterface).enableTexture(mGPUTextureId, textureUnit, mTextureData.mStage);
}

void GPUTexture::disable(u32 textureUnit) const
{
//    GET_SYSTEM(GPUInterface).disableTexture(textureUnit, mTextureData.mStage);
}

void GPUTexture::init(Ptr<GPUContext> gpuContext, const GPUTextureData& gpuTextureData, u32 id)
{
    mGPUContext = gpuContext;
    mTextureData = gpuTextureData;
    mID = id;

    if(gpuTextureData.mIsFont)
    {
        mWidth = mTextureData.mFontData.mWidth;
        mHeight = mTextureData.mFontData.mHeight;

//        mGPUTextureId = GET_SYSTEM(GPUInterface).createTexture1ByteChannel(mWidth, mHeight, nullptr);

        u32 texPos = 0;
        FOR_RANGE(c, 0, mTextureData.mFontData.mGlyphs.size())
        {
//            GET_SYSTEM(GPUInterface).setSubTexture(mGPUTextureId, texPos, 0, mTextureData.mFontData.mGlyphs[c].mBitmapSize.x, mTextureData.mFontData.mGlyphs[c].mBitmapSize.y, GPUTexturePixelFormat::RED, GPUPrimitiveDataType::UNSIGNED_BYTE, mTextureData.mFontData.mGlyphs[c].mData);
            // Increase texture offset
            texPos += mTextureData.mFontData.mGlyphs[c].mBitmapSize.x /*+ 2*/;
        }
    }
    else
    {
        ImageData imageData;
        imageData = ImageUtils::loadImage(gpuTextureData.mPath);
        mWidth = imageData.mWidth;
        mHeight = imageData.mHeight;
//        mGPUTextureId = GET_SYSTEM(GPUInterface).createTexture(GPUTextureFormat::RGBA8, mWidth, mHeight, true);
//        GET_SYSTEM(GPUInterface).setTextureData(mGPUTextureId, mWidth, mHeight, GPUTexturePixelFormat::RGBA, GPUPrimitiveDataType::UNSIGNED_BYTE, true, imageData.mData);
        CHECK_MSG(imageData.mData, "Error loading image " + mTextureData.mPath.get());
        ImageUtils::freeImage(imageData);
    }

    // Retrieve the texture handle after we finish creating the texture
//    mGPUTextureHandle = GET_SYSTEM(GPUInterface).getTextureHandle(mGPUTextureId);
    // CHECK_MSG(mGPUTextureHandle > 0, "TextureHandle error!");
//    GET_SYSTEM(GPUInterface).makeTextureResident(mGPUTextureHandle, true);

    vulkanTextureImage = new GPUImage();
    // vulkanTextureImage->init(mGPUContext->vulkanPhysicalDevice, mGPUContext->vulkanDevice);

    // if (!initializeTextureImage())
    // {
    //     CHECK_MSG(false,"Could not initialize texture image");
    // }
    // if (!initializeTextureImageView())
    // {
    //     CHECK_MSG(false,"Could not initialize texture image view");
    // }
    // if (!initializeTextureSampler())
    // {
    //     CHECK_MSG(false,"Could not initialize texture image sampler");
    // }
}

void GPUTexture::terminate() 
{
    if(mGPUTextureId > 0)
    {
//        GET_SYSTEM(GPUInterface).deleteTexture(mGPUTextureId);
        mGPUTextureId = 0;
        mGPUTextureHandle = 0;
    }

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroySampler(mGPUContext->vulkanDevice->getDevice(), textureSampler, allocationCallbacks);
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), textureImageView, allocationCallbacks);
    vulkanTextureImage->terminate();

}

bool GPUTexture::initializeTextureSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = mGPUContext->vulkanPhysicalDevice->getProperties().limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = (float) mipLevels;

        VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
        if (vkCreateSampler(mGPUContext->vulkanDevice->getDevice(), &samplerInfo, allocationCallbacks, &textureSampler) != VK_SUCCESS) {
            CHECK_MSG(false,"Could not create image sampler");
            return false;
        }
        return true;
    }

    bool GPUTexture::initializeTextureImageView() {
        textureImageView = GPUUtils::createImageView(mGPUContext, vulkanTextureImage->getVkImage(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
        if (!textureImageView) {
            CHECK_MSG(false,"Could not create Vulkan texture image view");
            return false;
        }
        return true;
    }
bool GPUTexture::initializeTextureImage() 
{

        /*
         * Load image texels
         */

        // int width;
        // int height;
        // int channels;
        // int desiredChannels = STBI_rgb_alpha;
        // stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &width, &height, &channels, desiredChannels);
        // if (!pixels) {
        //     CHECK_MSG(false,"Could not load texture image");
        //     return false;
        // }

        /*
         * This calculates the number of levels in the mip chain.
         * - The max function selects the largest dimension.
         * - The log2 function calculates how many times that dimension can be divided by 2.
         * - The floor function handles cases where the largest dimension is not a power of 2.
         * - 1 is added so that the original image has a mip level.
         */
        // mipLevels = ((uint32_t) std::floor(std::log2(std::max(width, height)))) + 1;

        /*
         * Copy image texels to staging buffer
         */

        VkDeviceSize imageSize = 0;//width * height * desiredChannels;
        GPUBuffer stagingBuffer;//(/*mGPUContext->vulkanPhysicalDevice, mGPUContext->vulkanDevice*/);

        GPUBufferData stagingBufferConfig{};
        stagingBufferConfig.Size = imageSize;
        stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        // if (!stagingBuffer.init(mGPUContext, stagingBufferConfig)) {
        //     CHECK_MSG(false,"Could not initialize texture image stagingBuffer");
        //     return false;
        // }

        // stagingBuffer.setData(pixels);
        // stbi_image_free(pixels);

        /*
         * Copy image texels from staging buffer to image
         */

        GPUImageData textureImageData{};
        // textureImageData.Width = width;
        // textureImageData.Height = height;
        textureImageData.Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageData.Format = VK_FORMAT_R8G8B8A8_SRGB;
        textureImageData.Tiling = VK_IMAGE_TILING_OPTIMAL;
        textureImageData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageData.Layout = VK_IMAGE_LAYOUT_UNDEFINED;
        textureImageData.MipLevels = mipLevels;
        textureImageData.SampleCount = VK_SAMPLE_COUNT_1_BIT;

        // if (!vulkanTextureImage->init(mGPUContext, textureImageData)) {
        //     CHECK_MSG(false,"Could not initialize texture image");
        //     return false;
        // }

        // VkImage textureImage = vulkanTextureImage->getVkImage();
        // if (!GET_SYSTEM(GPUInstance).transitionImageLayout(textureImage, textureImageData.Format, textureImageData.Layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureImageData.MipLevels)) {
        //     CHECK_MSG(false,"Could not transition image layout from undefined to transfer destination");
        //     return false;
        // }
        // copyBufferToImage(stagingBuffer.getVkBuffer(), textureImage, (uint32_t) width, (uint32_t) height);
        // stagingBuffer.terminate();

        // if (!generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, width, height, mipLevels)) {
        //     CHECK_MSG(false,"Could not generate mipmaps for texture image");
        //     return false;
        // }

        LOG("Initialized texture image");
        return true;
    }

    bool GPUTexture::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) const {

        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(mGPUContext->vulkanPhysicalDevice->getPhysicalDevice(), imageFormat, &formatProperties);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            CHECK_MSG(false,"Image format does not support linear blitting");
            return false;
        }

        VkCommandBuffer commandBuffer = GPUUtils::beginSingleTimeCommands(mGPUContext);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        constexpr VkDependencyFlags dependencyFlags = 0;
        constexpr uint32_t memoryBarrierCount = 0;
        constexpr VkMemoryBarrier* memoryBarriers = VK_NULL_HANDLE;
        constexpr uint32_t bufferMemoryBarrierCount = 0;
        constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = VK_NULL_HANDLE;
        constexpr uint32_t imageMemoryBarrierCount = 1;

        for (uint32_t i = 1; i < mipLevels; i++) {
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
            constexpr uint32_t regionCount = 1;
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

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
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

        GPUUtils::endSingleTimeCommands(mGPUContext, commandBuffer);
        return true;
    }
void GPUTexture::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const {
        VkCommandBuffer commandBuffer = GPUUtils::beginSingleTimeCommands(mGPUContext);

        VkBufferImageCopy bufferImageCopy{};
        bufferImageCopy.bufferOffset = 0;
        bufferImageCopy.bufferRowLength = 0;
        bufferImageCopy.bufferImageHeight = 0;

        bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferImageCopy.imageSubresource.mipLevel = 0;
        bufferImageCopy.imageSubresource.baseArrayLayer = 0;
        bufferImageCopy.imageSubresource.layerCount = 1;

        bufferImageCopy.imageOffset = {0, 0, 0};

        constexpr uint32_t depth = 1;
        bufferImageCopy.imageExtent = {width, height, depth };

        constexpr uint32_t regionCount = 1;
        constexpr VkImageLayout imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        vkCmdCopyBufferToImage(
                commandBuffer,
                buffer,
                image,
                imageLayout,
                regionCount,
                &bufferImageCopy
        );

        GPUUtils::endSingleTimeCommands(mGPUContext,commandBuffer);
    }
