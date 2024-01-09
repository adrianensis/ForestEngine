#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class GPUFramebuffer: public ObjectBase
{
    GENERATE_METADATA(GPUFramebuffer)

public:
    void init(const std::vector<GPUFramebufferAttachmentType>& attachments, u32 width, u32 height);
private:
    u32 mFramebufferId = 0;
    std::vector<GPUFramebufferAttachmentType> mAttachments;
    std::vector<u32> mAttachmentIDs;
    u32 mWidth = 0;
    u32 mHeight = 0;

public:
    GET(FramebufferId)
};
