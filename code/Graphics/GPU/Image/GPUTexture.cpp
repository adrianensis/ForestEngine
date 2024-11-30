#include "Graphics/GPU/Image/GPUTexture.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/GPU/Buffer/GPUBuffer.h"
#include "Graphics/GPU/Image/GPUImageUtils.hpp"

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
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // FONTS CASE
        // NEXT: Remove this. Work in progress case.
        return;
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        mImageData.mWidth = mTextureData.mFontData.mWidth;
        mImageData.mHeight = mTextureData.mFontData.mHeight;

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
        // ImageData imageData;
        mImageData = ImageUtils::loadImage(gpuTextureData.mPath);
//        mGPUTextureId = GET_SYSTEM(GPUInterface).createTexture(GPUTextureFormat::RGBA8, mWidth, mHeight, true);
//        GET_SYSTEM(GPUInterface).setTextureData(mGPUTextureId, mWidth, mHeight, GPUTexturePixelFormat::RGBA, GPUPrimitiveDataType::UNSIGNED_BYTE, true, imageData.mData);
        CHECK_MSG(mImageData.mData, "Error loading image " + mTextureData.mPath.get());
        // ImageUtils::freeImage(imageData);
    }

    // Retrieve the texture handle after we finish creating the texture
//    mGPUTextureHandle = GET_SYSTEM(GPUInterface).getTextureHandle(mGPUTextureId);
    // CHECK_MSG(mGPUTextureHandle > 0, "TextureHandle error!");
//    GET_SYSTEM(GPUInterface).makeTextureResident(mGPUTextureHandle, true);

    vulkanTextureImage = new GPUImage();
    // vulkanTextureImage->init(mGPUContext->vulkanPhysicalDevice, mGPUContext->vulkanDevice);

    if (!initializeTextureImage())
    {
        CHECK_MSG(false,"Could not initialize texture image");
    }
    if (!initializeTextureImageView())
    {
        CHECK_MSG(false,"Could not initialize texture image view");
    }
    if (!initializeTextureSampler())
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
        samplerInfo.maxLod = (float) mMipMapLevel;

        VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
        if (vkCreateSampler(mGPUContext->vulkanDevice->getDevice(), &samplerInfo, allocationCallbacks, &textureSampler) != VK_SUCCESS) {
            CHECK_MSG(false,"Could not create image sampler");
            return false;
        }
        return true;
    }

    bool GPUTexture::initializeTextureImageView() {
        textureImageView = GPUImageUtils::createImageView(mGPUContext, vulkanTextureImage->getVkImage(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mMipMapLevel);
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
        mMipMapLevel = ((uint32_t) std::floor(std::log2(std::max(mImageData.mWidth, mImageData.mHeight)))) + 1;

        /*
         * Copy image texels to staging buffer
         */

        VkDeviceSize imageSize = mImageData.mWidth * mImageData.mHeight * 4;//STBI_rgb_alpha;
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

        GPUImageData textureImageData{};
        textureImageData.Width = mImageData.mWidth;
        textureImageData.Height = mImageData.mHeight;
        textureImageData.Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageData.Format = VK_FORMAT_R8G8B8A8_SRGB;
        textureImageData.Tiling = VK_IMAGE_TILING_OPTIMAL;
        textureImageData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageData.Layout = VK_IMAGE_LAYOUT_UNDEFINED;
        textureImageData.MipLevels = mMipMapLevel;
        textureImageData.SampleCount = VK_SAMPLE_COUNT_1_BIT;

        if (!vulkanTextureImage->init(mGPUContext, textureImageData)) {
            CHECK_MSG(false,"Could not initialize texture image");
            return false;
        }

        VkImage textureImage = vulkanTextureImage->getVkImage();
        if (!GPUImageUtils::transitionImageLayout(mGPUContext, textureImage, textureImageData.Format, textureImageData.Layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureImageData.MipLevels)) {
            CHECK_MSG(false,"Could not transition image layout from undefined to transfer destination");
            return false;
        }
        GPUImageUtils::copyBufferToImage(mGPUContext, stagingBuffer.getVkBuffer(), textureImage, mImageData.mWidth, mImageData.mHeight);
        stagingBuffer.terminate();

        if (!GPUImageUtils::generateMipmaps(mGPUContext, mImageData, textureImage, VK_FORMAT_R8G8B8A8_SRGB, mMipMapLevel)) {
            CHECK_MSG(false,"Could not generate mipmaps for texture image");
            return false;
        }

        LOG("Initialized texture image");
        return true;
    }