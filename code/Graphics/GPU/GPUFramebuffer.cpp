#include "Graphics/GPU/GPUFramebuffer.hpp"

void GPUFramebuffer::init(const GPUFramebufferData& framebufferData)
{
    mFrameBufferData = framebufferData;

    mFramebufferId = GET_SYSTEM(GPUInterface).createFramebuffer(mFrameBufferData.mWidth, mFrameBufferData.mHeight);
    GET_SYSTEM(GPUInterface).enableFramebuffer(GPUFramebufferOperationType::READ_AND_DRAW, mFramebufferId);

    FOR_LIST(it, mFrameBufferData.mAttachments)
    {
        const GPUFramebufferAttachmentType& attachmentType = *it;
        u32 id = GET_SYSTEM(GPUInterface).createFramebufferAttachment(attachmentType, mFrameBufferData.mWidth, mFrameBufferData.mHeight);
        mAttachmentIDs.push_back(id);
    }

    GET_SYSTEM(GPUInterface).checkFramebufferErrors();
    GET_SYSTEM(GPUInterface).disableFramebuffer(GPUFramebufferOperationType::READ_AND_DRAW);
}

Vector4 GPUFramebuffer::readPixel(u32 x, u32 y, GPUFramebufferAttachmentType attachmentType) const
{
    CHECK_MSG(mFrameBufferData.mAttachments.contains(attachmentType), "Attachment not found!");

    GET_SYSTEM(GPUInterface).enableFramebuffer(GPUFramebufferOperationType::READ, mFramebufferId);
    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(attachmentType);
    Vector4 pixelColor = GET_SYSTEM(GPUInterface).readFramebufferPixel(x, y, GPUTexturePixelFormat::RGBA);
    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(GPUFramebufferAttachmentType::NONE);
    GET_SYSTEM(GPUInterface).disableFramebuffer(GPUFramebufferOperationType::READ);
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