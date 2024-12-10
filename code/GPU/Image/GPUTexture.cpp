#include "GPU/Image/GPUTexture.hpp"
#include "GPU/GPUInstance.hpp"
#include "GPU/Buffer/GPUBuffer.h"
#include "GPU/Image/GPUImageUtils.hpp"

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

    mVulkanTextureImage = new GPUImage();
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

    if(gpuTextureData.mIsFont)
    {
        format = VK_FORMAT_R8_SRGB;
        mChannels = 1;
        mImageData.mWidth = mTextureData.mFontData.mWidth;
        mImageData.mHeight = mTextureData.mFontData.mHeight;
        mImageData.mData = new byte[mImageData.mWidth * mImageData.mHeight * mChannels /*1 channel*/];
    }
    else
    {
        format = VK_FORMAT_R8G8B8A8_SRGB;
        mChannels = 4;
        mImageData = ImageUtils::loadImage(gpuTextureData.mPath);
        CHECK_MSG(mImageData.mData, "Error loading image " + mTextureData.mPath.get());


        /*
        * This calculates the number of levels in the mip chain.
        * - The max function selects the largest dimension.
        * - The log2 function calculates how many times that dimension can be divided by 2.
        * - The floor function handles cases where the largest dimension is not a power of 2.
        * - 1 is added so that the original image has a mip level.
        */
        mMipMapLevels = ((uint32_t) std::floor(std::log2(std::max(mImageData.mWidth, mImageData.mHeight)))) + 1;

    }

    if(gpuTextureData.mIsFont)
    {
//        mGPUTextureId = GET_SYSTEM(GPUInterface).createTexture1ByteChannel(mWidth, mHeight, nullptr);

        CHECK_MSG(!mTextureData.mFontData.mGlyphs.empty(), "Error empty font " + mTextureData.mFontData.mPath.get());

        GPUImageData textureImageData{};
        textureImageData.Width = mImageData.mWidth;
        textureImageData.Height = mImageData.mHeight;
        textureImageData.Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageData.Format = format;
        textureImageData.Tiling = VK_IMAGE_TILING_LINEAR;
        textureImageData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageData.Layout = VK_IMAGE_LAYOUT_UNDEFINED;
        textureImageData.MipLevels = mMipMapLevels;
        textureImageData.SampleCount = VK_SAMPLE_COUNT_1_BIT;
        textureImageData.mOffsetX = 0;
        textureImageData.mOffsetY = 0;
        textureImageData.mChannels = mChannels;
        if (!mVulkanTextureImage->init(mGPUContext, textureImageData)) {
            CHECK_MSG(false,"Could not initialize texture image");
        }

//         u32 texPos = 0;
//         FOR_RANGE(c, 0, mTextureData.mFontData.mGlyphs.size())
//         {
// //            GET_SYSTEM(GPUInterface).setSubTexture(mGPUTextureId, texPos, 0, mTextureData.mFontData.mGlyphs[c].mBitmapSize.x, mTextureData.mFontData.mGlyphs[c].mBitmapSize.y, GPUTexturePixelFormat::RED, GPUPrimitiveDataType::UNSIGNED_BYTE, mTextureData.mFontData.mGlyphs[c].mData);
//             // Increase texture offset



//             GPUImageData subtextureImageData{};
//             subtextureImageData.Width = mTextureData.mFontData.mGlyphs[c].mBitmapSize.x;
//             subtextureImageData.Height = mTextureData.mFontData.mGlyphs[c].mBitmapSize.y;
//             // TODO: " " space case! no size, no data, not supported by vulkan
            
//             subtextureImageData.Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
//             subtextureImageData.Format = format;
//             subtextureImageData.Tiling = VK_IMAGE_TILING_LINEAR;
//             subtextureImageData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
//             subtextureImageData.Layout = VK_IMAGE_LAYOUT_UNDEFINED;
//             subtextureImageData.MipLevels = mMipMapLevels;
//             subtextureImageData.SampleCount = VK_SAMPLE_COUNT_1_BIT;
//             subtextureImageData.mOffsetX = texPos;
//             subtextureImageData.mOffsetY = 0;
//             subtextureImageData.mChannels = mChannels;

//             if(subtextureImageData.Width + subtextureImageData.Height == 0)
//             {
//                 continue;
//             }
//             texPos += mTextureData.mFontData.mGlyphs[c].mBitmapSize.x /*+ 2*/;

//             if (!initializeTextureImage(subtextureImageData))
//             {
//                 CHECK_MSG(false,"Could not initialize texture image");
//             }

//             break;
            
//         }
    }
    else
    {
        // ImageUtils::freeImage(imageData);

        GPUImageData textureImageData{};
        textureImageData.Width = mImageData.mWidth;
        textureImageData.Height = mImageData.mHeight;
        textureImageData.Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageData.Format = format;
        textureImageData.Tiling = VK_IMAGE_TILING_OPTIMAL;
        textureImageData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageData.Layout = VK_IMAGE_LAYOUT_UNDEFINED;
        textureImageData.MipLevels = mMipMapLevels;
        textureImageData.SampleCount = VK_SAMPLE_COUNT_1_BIT;
        textureImageData.mOffsetX = 0;
        textureImageData.mOffsetY = 0;
        textureImageData.mChannels = mChannels;

        if (!mVulkanTextureImage->init(mGPUContext, textureImageData)) {
            CHECK_MSG(false,"Could not initialize texture image");
        }

        if (!initializeTextureImage(textureImageData))
        {
            CHECK_MSG(false,"Could not initialize texture image");
        }
    }

    if (!initializeTextureImageView(format, VK_IMAGE_ASPECT_COLOR_BIT))
    {
        CHECK_MSG(false,"Could not initialize texture image view");
    }

    VkSamplerCreateInfo samplerInfo = {};
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
    samplerInfo.maxLod = (float) mMipMapLevels;
    if (!initializeTextureSampler(samplerInfo))
    {
        CHECK_MSG(false,"Could not initialize texture image sampler");
    }

    ImageUtils::freeImage(mImageData);
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
    vkDestroySampler(mGPUContext->vulkanDevice->getDevice(), mTextureSampler, allocationCallbacks);
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), mTextureImageView, allocationCallbacks);
    mVulkanTextureImage->terminate();

}

bool GPUTexture::initializeTextureSampler(const VkSamplerCreateInfo& samplerInfo)
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    if (vkCreateSampler(mGPUContext->vulkanDevice->getDevice(), &samplerInfo, allocationCallbacks, &mTextureSampler) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create image sampler");
        return false;
    }
    return true;
}

bool GPUTexture::initializeTextureImageView(VkFormat format, VkImageAspectFlagBits imageAspectFlagBits) {
    mTextureImageView = GPUImageUtils::createImageView(mGPUContext, mVulkanTextureImage->getVkImage(), format, imageAspectFlagBits, mMipMapLevels);
    if (!mTextureImageView) {
        CHECK_MSG(false,"Could not create Vulkan texture image view");
        return false;
    }
    return true;
}
bool GPUTexture::initializeTextureImage(const GPUImageData& textureImageData) 
{
        /*
         * Copy image texels to staging buffer
         */

        VkDeviceSize imageSize = mImageData.mWidth * mImageData.mHeight * textureImageData.mChannels;//STBI_rgb_alpha;
        GPUBuffer stagingBuffer;//(/*mGPUContext->vulkanPhysicalDevice, mGPUContext->vulkanDevice*/);

        GPUBufferData stagingBufferConfig{};
        stagingBufferConfig.Size = imageSize;
        stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (!stagingBuffer.init(mGPUContext, stagingBufferConfig)) {
            CHECK_MSG(false,"Could not initialize texture image stagingBuffer");
            return false;
        }

        stagingBuffer.setData(mImageData.mData);
        // stbi_image_free(pixels);

        /*
         * Copy image texels from staging buffer to image
         */

        VkImage textureImage = mVulkanTextureImage->getVkImage();
        if (!GPUImageUtils::transitionImageLayout(mGPUContext, textureImage, textureImageData.Format, textureImageData.Layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureImageData.MipLevels)) {
            CHECK_MSG(false,"Could not transition image layout from undefined to transfer destination");
            return false;
        }
        GPUImageUtils::copyBufferToImage(mGPUContext, stagingBuffer.getVkBuffer(), textureImage, textureImageData.Width, textureImageData.Height, textureImageData.mOffsetX, textureImageData.mOffsetY);
        stagingBuffer.terminate();

        if (!GPUImageUtils::generateMipmaps(mGPUContext, mImageData, textureImage, textureImageData.Format, textureImageData.MipLevels)) {
            CHECK_MSG(false,"Could not generate mipmaps for texture image");
            return false;
        }

        LOG("Initialized texture image");
        return true;
    }