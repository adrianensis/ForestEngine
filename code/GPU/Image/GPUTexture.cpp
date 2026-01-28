#include "GPU/Image/GPUTexture.hpp"
#include "GPU/Buffer/GPUBuffer.h"
#include "GPU/Image/GPUImageUtils.hpp"
#include "vulkan/vulkan_core.h"

void GPUTexture::init(GPUContext* gpuContext, const GPUTextureData& gpuTextureData, Core::u32 id)
{
    PROFILER_CPU_NAMED(init_texture)
    mGPUContext = gpuContext;
    mTextureData = gpuTextureData;
    mID = id;

    if(gpuTextureData.mIsFont)
    {
        mFormat = VK_FORMAT_R8_SRGB;
        mImageData.mWidth = mTextureData.mFontData.mWidth;
        mImageData.mHeight = mTextureData.mFontData.mHeight;
    }
    else
    {
        if(gpuTextureData.mIsLinearData)
        {
            // Use UNORM for Normal maps, Roughness, Metallic, AO
            mFormat = VK_FORMAT_R8G8B8A8_UNORM;
        }
        else
        {
            // Use SRGB for Base Color / Albedo / Emissive
            mFormat = VK_FORMAT_R8G8B8A8_SRGB;
        }

        PROFILER_CPU_NAMED(load_image)
        mImageData = Image::ImageUtils::loadImage(gpuTextureData.mPath);
        CHECK_MSG(mImageData.mData, "Error loading image " + mTextureData.mPath.get());

        /*
        * This calculates the number of levels in the mip chain.
        * - The max function selects the largest dimension.
        * - The log2 function calculates how many times that dimension can be divided by 2.
        * - The floor function handles cases where the largest dimension is not a power of 2.
        * - 1 is added so that the original image has a mip level.
        */
        mMipMapLevels = ((Core::u32) std::floor(std::log2(std::max(mImageData.mWidth, mImageData.mHeight)))) + 1;
    }

    if(gpuTextureData.mIsFont)
    {
        PROFILER_CPU_NAMED(init_texture_font)

        CHECK_MSG(!mTextureData.mFontData.mGlyphs.empty(), "Error empty font " + mTextureData.mFontData.mPath.get());

        GPUImageData textureImageData{};
        textureImageData.Width = mImageData.mWidth;
        textureImageData.Height = mImageData.mHeight;
        textureImageData.Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageData.Format = mFormat;
        textureImageData.Tiling = VK_IMAGE_TILING_LINEAR;
        textureImageData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageData.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        textureImageData.MipLevels = mMipMapLevels;
        textureImageData.SampleCount = VK_SAMPLE_COUNT_1_BIT;
        textureImageData.mOffsetX = 0;
        textureImageData.mOffsetY = 0;
        textureImageData.mChannels = TO_U32(GPUTextureChannels::ONE);
        if (!mVulkanTextureImage.init(mGPUContext, textureImageData)) {
            CHECK_MSG(false,"Could not initialize texture image");
        }

        if (!GPUImageUtils::createTextureImage(mGPUContext, mVulkanTextureImage.getVkImage(), textureImageData, mTextureData.mFontData.mGlyphAtlasData))
        {
            CHECK_MSG(false,"Could not initialize texture image");
        }
    }
    else
    {
        PROFILER_CPU_NAMED(init_texture_common)

        // ImageUtils::freeImage(imageData);

        GPUImageData textureImageData{};
        textureImageData.Width = mImageData.mWidth;
        textureImageData.Height = mImageData.mHeight;
        textureImageData.Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageData.Format = mFormat;
        textureImageData.Tiling = VK_IMAGE_TILING_OPTIMAL;
        textureImageData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageData.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        textureImageData.MipLevels = mMipMapLevels;
        textureImageData.SampleCount = VK_SAMPLE_COUNT_1_BIT;
        textureImageData.mOffsetX = 0;
        textureImageData.mOffsetY = 0;
        textureImageData.mChannels = TO_U32(GPUTextureChannels::FOUR);

        if (!mVulkanTextureImage.init(mGPUContext, textureImageData)) {
            CHECK_MSG(false,"Could not initialize texture image");
        }

        if (!GPUImageUtils::createTextureImage(mGPUContext, mVulkanTextureImage.getVkImage(), textureImageData, mImageData.mData))
        {
            CHECK_MSG(false,"Could not initialize texture image");
        }

        Image::ImageUtils::freeImage(mImageData);
    }

    mTextureImageView = GPUImageUtils::createImageView(mGPUContext, mVulkanTextureImage.getVkImage(), mFormat, VK_IMAGE_ASPECT_COLOR_BIT, mMipMapLevels);
    if (!mTextureImageView)
    {
        CHECK_MSG(false,"Could not create Vulkan texture image view");
    }

    {
        PROFILER_CPU_NAMED(init_texture_sampler)

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = mGPUContext->vulkanPhysicalDevice->getProperties().properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = (float) mMipMapLevels;

        VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
        if (vkCreateSampler(mGPUContext->vulkanDevice->getDevice(), &samplerInfo, allocationCallbacks, &mTextureSampler) != VK_SUCCESS)
        {
            CHECK_MSG(false,"Could not create image sampler");
        }
    }
}

void GPUTexture::terminate() 
{
    PROFILER_CPU_NAMED(terminate_texture)

    if(mGPUTextureId > 0)
    {
        mGPUTextureId = 0;
        mGPUTextureHandle = 0;
    }

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroySampler(mGPUContext->vulkanDevice->getDevice(), mTextureSampler, allocationCallbacks);
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), mTextureImageView, allocationCallbacks);
    mVulkanTextureImage.terminate();

}