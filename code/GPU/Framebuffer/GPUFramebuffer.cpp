#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/Image/GPUImageUtils.hpp"

Maths::Vector4 GPUFramebuffer::readPixel(Core::u32 x, Core::u32 y, GPUFramebufferAttachmentType attachmentType) const
{
    CHECK_MSG(mFramebufferData.mAttachments.contains(attachmentType), "Attachment not found!");

//    // GET_SYSTEM(GPUInterface).enableFramebuffer(GPUFramebufferOperationType::READ, mFramebufferId);
//    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(attachmentType);
//    Maths::Vector4 pixelColor = GET_SYSTEM(GPUInterface).readFramebufferPixel(x, y, GPUTexturePixelFormat::RGBA);
//    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(GPUFramebufferAttachmentType::NONE);
//    // GET_SYSTEM(GPUInterface).disableFramebuffer(GPUFramebufferOperationType::READ);
    // return pixelColor;
    return Maths::Vector4();
}

void GPUFramebuffer::enable(GPUFramebufferOperationType op)
{
//    GET_SYSTEM(GPUInterface).enableFramebuffer(op, mFramebufferId);
}

void GPUFramebuffer::disable(GPUFramebufferOperationType op)
{
//    GET_SYSTEM(GPUInterface).disableFramebuffer(op);
}

const VkFramebuffer GPUFramebuffer::getFramebuffer() const {
    return framebuffer;
}

bool GPUFramebuffer::init(GPUContext* gpuContext, const GPUFramebufferData& framebufferData, GPURenderPass* renderPass)
{
    mGPUContext = gpuContext;
    mRenderPass = renderPass;
    mFramebufferData = framebufferData;

    if (!initializeColorResources())
    {
        CHECK_MSG(false, "Could not initialize color resources");
    }
    if (!initializeDepthResources())
    {
        CHECK_MSG(false, "Could not initialize depth resources");
    }

//    mFramebufferId = GET_SYSTEM(GPUInterface).createFramebuffer(mFramebufferData.mWidth, mFramebufferData.mHeight);
    FOR_LIST(it, mFramebufferData.mAttachments)
    {
        const GPUFramebufferAttachmentType& attachmentType = *it;
//        Core::u32 attachmentID = GET_SYSTEM(GPUInterface).createFramebufferAttachment(mFramebufferId, attachmentType, mFramebufferData.mWidth, mFramebufferData.mHeight);
        // mAttachments.insert_or_assign(attachmentType, GPUFramebufferAttachment{attachmentType, attachmentID});
    }

//    GET_SYSTEM(GPUInterface).checkFramebufferErrors();

    std::vector<VkImageView> attachments =
    {
        colorImageView,
        depthImageView
    };
    // if(mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT)
    // {
    //     attachments =
    //     {
    //         colorImageView,
    //         depthImageView
    //     };
    // }
    // else
    // {
    //     attachments =
    //     {
    //         colorImageView,
    //         depthImageView,
    //         swapChainImageView
    //     };
    // }

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

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = mRenderPass->getRenderPass();
    framebufferInfo.attachmentCount = (Core::u32) attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = mGPUContext->vulkanSwapChain->getExtent().width;
    framebufferInfo.height = mGPUContext->vulkanSwapChain->getExtent().height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(mGPUContext->vulkanDevice->getDevice(), &framebufferInfo, ALLOCATOR, &framebuffer) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan framebuffer");
        return false;
    }
    return true;
}

void GPUFramebuffer::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), colorImageView, allocationCallbacks);
    // vulkanColorImage.terminate();
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

    // GPUImageData colorImageConfig{};
    // colorImageConfig.Width = mGPUContext->vulkanSwapChain->getExtent().width;
    // colorImageConfig.Height = mGPUContext->vulkanSwapChain->getExtent().height;
    // colorImageConfig.MipLevels = 1;
    // colorImageConfig.SampleCount = mFramebufferData.mSampleCountFlagBits;
    // colorImageConfig.Format = colorFormat;
    // colorImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    // colorImageConfig.Usage = VK_IMAGE_USAGE_SAMPLED_BIT|/*VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |*/ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT /*| VK_IMAGE_USAGE_TRANSFER_DST_BIT*/;
    // colorImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    // colorImageConfig.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // if (!vulkanColorImage.init(mGPUContext, colorImageConfig)) {
    //     CHECK_MSG(false,"Could not initialize color image");
    //     return false;
    // }
    colorImageView = GPUImageUtils::createImageView(mGPUContext, mFramebufferData.mColorImage->getVkImage(), colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, mFramebufferData.mColorImage->getGPUImageData().MipLevels);
    
    // if(!mFramebufferData.mIsResolveFramebuffer)
    // {
    //     vulkanColorImage.transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    // }
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
    // GPUImageUtils::transitionImageLayout(mGPUContext, vulkanDepthImage.getVkImage(), depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, depthImageConfig.MipLevels);
    return true;
}