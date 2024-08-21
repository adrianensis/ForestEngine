#include "Graphics/GPU/GPUFramebuffer.hpp"

void GPUFramebuffer::init(const GPUFramebufferData& framebufferData)
{
    mFramebufferData = framebufferData;
//    mFramebufferId = GET_SYSTEM(GPUInterface).createFramebuffer(mFramebufferData.mWidth, mFramebufferData.mHeight);
    FOR_LIST(it, mFramebufferData.mAttachments)
    {
        const GPUFramebufferAttachmentType& attachmentType = *it;
//        u32 attachmentID = GET_SYSTEM(GPUInterface).createFramebufferAttachment(mFramebufferId, attachmentType, mFramebufferData.mWidth, mFramebufferData.mHeight);
        // mAttachments.insert_or_assign(attachmentType, GPUFramebufferAttachment{attachmentType, attachmentID});
    }

//    GET_SYSTEM(GPUInterface).checkFramebufferErrors();
}

Vector4 GPUFramebuffer::readPixel(u32 x, u32 y, GPUFramebufferAttachmentType attachmentType) const
{
    CHECK_MSG(mFramebufferData.mAttachments.contains(attachmentType), "Attachment not found!");

//    // GET_SYSTEM(GPUInterface).enableFramebuffer(GPUFramebufferOperationType::READ, mFramebufferId);
//    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(attachmentType);
//    Vector4 pixelColor = GET_SYSTEM(GPUInterface).readFramebufferPixel(x, y, GPUTexturePixelFormat::RGBA);
//    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(GPUFramebufferAttachmentType::NONE);
//    // GET_SYSTEM(GPUInterface).disableFramebuffer(GPUFramebufferOperationType::READ);
    // return pixelColor;
    return Vector4();
}

void GPUFramebuffer::enable(GPUFramebufferOperationType op)
{
//    GET_SYSTEM(GPUInterface).enableFramebuffer(op, mFramebufferId);
}

void GPUFramebuffer::disable(GPUFramebufferOperationType op)
{
//    GET_SYSTEM(GPUInterface).disableFramebuffer(op);
}

const VkAllocationCallbacks* GPUFramebuffer::ALLOCATOR = VK_NULL_HANDLE;

const VkFramebuffer GPUFramebuffer::getFramebuffer() const {
    return framebuffer;
}

bool GPUFramebuffer::initialize(Ptr<GPUContext> gpuContext, GPURenderPass* vulkanRenderPasssss, VkImageView colorImageView, VkImageView depthImageView, VkImageView swapChainImageView) {
    std::array<VkImageView, 3> attachments[] = {
            colorImageView,
            depthImageView,
            swapChainImageView
    };

    mGPUContext = gpuContext;
    vulkanRenderPass = vulkanRenderPasssss;

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vulkanRenderPass->getRenderPass();
    framebufferInfo.attachmentCount = (uint32_t) attachments->size();
    framebufferInfo.pAttachments = attachments->data();
    framebufferInfo.width = mGPUContext->vulkanSwapChain->getExtent().width;
    framebufferInfo.height = mGPUContext->vulkanSwapChain->getExtent().height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(mGPUContext->vulkanDevice->getDevice(), &framebufferInfo, ALLOCATOR, &framebuffer) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan framebuffer");
        return false;
    }
    return true;
}

void GPUFramebuffer::terminate() {
    vkDestroyFramebuffer(mGPUContext->vulkanDevice->getDevice(), framebuffer, ALLOCATOR);
}