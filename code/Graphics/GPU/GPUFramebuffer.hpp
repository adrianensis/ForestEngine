#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class GPUFramebufferAttachmentData
{
public:
    GPUFramebufferAttachmentType mAttachmentType;
};

class GPUFramebuffer
{
public:
    void init(const std::vector<GPUFramebufferAttachmentData>& attachments, u32 width, u32 height);
    Vector4 readPixel(u32 x, u32 y, GPUFramebufferAttachmentType attachmentType) const;
private:
    u32 mFramebufferId = 0;
    std::unordered_map<GPUFramebufferAttachmentType, GPUFramebufferAttachmentData> mAttachments;
    std::vector<u32> mAttachmentIDs;
    u32 mWidth = 0;
    u32 mHeight = 0;

public:
    GET(FramebufferId)
};
