#include "Graphics/GPU/GPUFramebuffer.hpp"

void GPUFramebuffer::init(const std::vector<GPUFramebufferAttachmentType>& attachments, u32 width, u32 height)
{
    mWidth = width;
    mHeight = height;
    mAttachments = attachments;

    mFramebufferId = GET_SYSTEM(GPUInterface).createFramebuffer(mWidth, mHeight);
    GET_SYSTEM(GPUInterface).enableFramebuffer(mFramebufferId);

    FOR_ARRAY(i, mAttachments)
    {
        GPUFramebufferAttachmentType attachmentType = mAttachments[i];
        u32 id = GET_SYSTEM(GPUInterface).createFramebufferAttachment(attachmentType, mWidth, mHeight);
        mAttachmentIDs.push_back(id);
    }

    GET_SYSTEM(GPUInterface).checkFramebufferErrors();
    GET_SYSTEM(GPUInterface).disableFramebuffer();
}
