#include "Graphics/GPU/GPUFramebuffer.hpp"

void GPUFramebuffer::init(const std::vector<GPUFramebufferAttachmentData>& attachments, u32 width, u32 height)
{
    mWidth = width;
    mHeight = height;
    
    FOR_ARRAY(i, attachments)
    {
        const GPUFramebufferAttachmentData& attachmentData = attachments[i];
        CHECK_MSG(!mAttachments.contains(attachmentData.mAttachmentType), "Duplicated Attachment!");
        mAttachments.emplace(attachmentData.mAttachmentType, attachmentData);
    }

    mFramebufferId = GET_SYSTEM(GPUInterface).createFramebuffer(mWidth, mHeight);
    GET_SYSTEM(GPUInterface).enableFramebuffer(GPUFramebufferOperationType::READ_AND_DRAW, mFramebufferId);

    FOR_MAP(it, mAttachments)
    {
        const GPUFramebufferAttachmentData& attachmentData = it->second;
        u32 id = GET_SYSTEM(GPUInterface).createFramebufferAttachment(attachmentData.mAttachmentType, mWidth, mHeight);
        mAttachmentIDs.push_back(id);
    }

    GET_SYSTEM(GPUInterface).checkFramebufferErrors();
    GET_SYSTEM(GPUInterface).disableFramebuffer(GPUFramebufferOperationType::READ_AND_DRAW);
}

Vector4 GPUFramebuffer::readPixel(u32 x, u32 y, GPUFramebufferAttachmentType attachmentType) const
{
    CHECK_MSG(mAttachments.contains(attachmentType), "Attachment not found!");
    const GPUFramebufferAttachmentData& attachmentData = mAttachments.at(attachmentType);
    GET_SYSTEM(GPUInterface).enableFramebuffer(GPUFramebufferOperationType::READ, mFramebufferId);
    GET_SYSTEM(GPUInterface).setFramebufferAttachmentToRead(attachmentData.mAttachmentType);
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