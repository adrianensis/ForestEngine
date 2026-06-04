#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/Image/GPUImageUtils.hpp"

Maths::Vector4 GPUFramebuffer::readPixel(GPU::u32 x, GPU::u32 y, GPUFramebufferAttachmentType attachmentType) const
{
    CHECK_MSG(mFramebufferData.mAttachments.contains(attachmentType), "Attachment not found!");

//    // (GPUInterface).enableFramebuffer(GPUFramebufferOperationType::READ, mFramebufferId);
//    (GPUInterface).setFramebufferAttachmentToRead(attachmentType);
//    Maths::Vector4 pixelColor = (GPUInterface).readFramebufferPixel(x, y, GPUTexturePixelFormat::RGBA);
//    (GPUInterface).setFramebufferAttachmentToRead(GPUFramebufferAttachmentType::NONE);
//    // (GPUInterface).disableFramebuffer(GPUFramebufferOperationType::READ);
    // return pixelColor;
    return Maths::Vector4();
}

void GPUFramebuffer::enable(GPUFramebufferOperationType op)
{
}

void GPUFramebuffer::disable(GPUFramebufferOperationType op)
{
}

const VkFramebuffer GPUFramebuffer::getFramebuffer() const {
    return framebuffer;
}

bool GPUFramebuffer::init(GPUContext* gpuContext, const GPUFramebufferData& framebufferData)
{
    mGPUContext = gpuContext;
    mFramebufferData = framebufferData;

    if (!initializeColorResources())
    {
        CHECK_MSG(false, "Could not initialize color resources");
    }
    if (!initializeDepthResources())
    {
        CHECK_MSG(false, "Could not initialize depth resources");
    }

//    mFramebufferId = (GPUInterface).createFramebuffer(mFramebufferData.mWidth, mFramebufferData.mHeight);
    FOR_LIST(it, mFramebufferData.mAttachments)
    {
        const GPUFramebufferAttachmentType& attachmentType = *it;
//        GPU::u32 attachmentID = (GPUInterface).createFramebufferAttachment(mFramebufferId, attachmentType, mFramebufferData.mWidth, mFramebufferData.mHeight);
        // mAttachments.insert_or_assign(attachmentType, GPUFramebufferAttachment{attachmentType, attachmentID});
    }

//    (GPUInterface).checkFramebufferErrors();

    std::vector<VkImageView> attachments =
    {
        colorImageView,
        depthImageView
    };

    if(mFramebufferData.mIsResolveFramebuffer)
    {
        if(mFramebufferData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT)
        {
            attachments[0] = mGPUContext->vulkanSwapChain->getImageViews()[mFramebufferData.mSwapchainIndex];
        }
        else
        {
            attachments.push_back(mGPUContext->vulkanSwapChain->getImageViews()[mFramebufferData.mSwapchainIndex]);
        }
    }

    return true;
}

void GPUFramebuffer::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), colorImageView, allocationCallbacks);
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), depthImageView, allocationCallbacks);
    vulkanDepthImage.terminate();

    vkDestroyFramebuffer(mGPUContext->vulkanDevice->getDevice(), framebuffer, ALLOCATOR);
}

bool GPUFramebuffer::initializeColorResources()
{
    PROFILER_CPU()

    if(! mFramebufferData.mColorImage)
    {
        return true;
    }

    VkFormat colorFormat = mGPUContext->vulkanSwapChain->getSurfaceFormat().format;
    colorImageView = GPUImageUtils::createImageView(mGPUContext, mFramebufferData.mColorImage->getVkImage(), colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, mFramebufferData.mColorImage->getGPUImageData().MipLevels);

    return true;
}

bool GPUFramebuffer::initializeDepthResources()
{
    PROFILER_CPU()

    VkFormat depthFormat = GPUImageUtils::findDepthFormat(mGPUContext);

    GPUImageData depthImageConfig{};
    depthImageConfig.Width = mGPUContext->vulkanSwapChain->getExtent().width;
    depthImageConfig.Height = mGPUContext->vulkanSwapChain->getExtent().height;
    depthImageConfig.Format = depthFormat;
    depthImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImageConfig.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    depthImageConfig.MipLevels = 1;
    depthImageConfig.SampleCount = mFramebufferData.mSampleCountFlagBits;
    depthImageConfig.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (!vulkanDepthImage.init(mGPUContext, depthImageConfig)) {
        CHECK_MSG(false,"Could not initialize depth image");
        return false;
    }
    depthImageView = GPUImageUtils::createImageView(mGPUContext, vulkanDepthImage.getVkImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, depthImageConfig.MipLevels);
    return true;
}