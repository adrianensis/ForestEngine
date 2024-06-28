#include "Graphics/GPU/GPUFramebuffer.hpp"

void GPUFramebuffer::init(const GPUFramebufferData& framebufferData)
{
    mFramebufferData = framebufferData;
    mFramebufferId = GET_SYSTEM(GPUInterface).createFramebuffer(mFramebufferData.mWidth, mFramebufferData.mHeight);
    FOR_LIST(it, mFramebufferData.mAttachments)
    {
        const GPUFramebufferAttachmentType& attachmentType = *it;
        u32 attachmentID = GET_SYSTEM(GPUInterface).createFramebufferAttachment(mFramebufferId, attachmentType, mFramebufferData.mWidth, mFramebufferData.mHeight);
        mAttachments.insert_or_assign(attachmentType, GPUFramebufferAttachment{attachmentType, attachmentID});
    }

    GET_SYSTEM(GPUInterface).checkFramebufferErrors();
}

Vector4 GPUFramebuffer::readPixel(u32 x, u32 y, GPUFramebufferAttachmentType attachmentType) const
{
    CHECK_MSG(mFramebufferData.mAttachments.contains(attachmentType), "Attachment not found!");

    // GET_SYSTEM(GPUInterface).enableFramebuffer(GPUFramebufferOperationType::READ, mFramebufferId);
    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(attachmentType);
    Vector4 pixelColor = GET_SYSTEM(GPUInterface).readFramebufferPixel(x, y, GPUTexturePixelFormat::RGBA);
    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(GPUFramebufferAttachmentType::NONE);
    // GET_SYSTEM(GPUInterface).disableFramebuffer(GPUFramebufferOperationType::READ);
    return pixelColor;
}

void GPUFramebuffer::enable(GPUFramebufferOperationType op)
{
    GET_SYSTEM(GPUInterface).enableFramebuffer(op, mFramebufferId);
}

void GPUFramebuffer::disable(GPUFramebufferOperationType op)
{
    GET_SYSTEM(GPUInterface).disableFramebuffer(op);
}