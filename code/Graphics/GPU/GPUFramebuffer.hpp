#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class GPUFramebufferData
{
public:
    std::unordered_set<GPUFramebufferAttachmentType> mAttachments;
    u32 mWidth = 0;
    u32 mHeight = 0;
};

class GPUFramebuffer
{
public:
    void init(const GPUFramebufferData& framebufferData);
    Vector4 readPixel(u32 x, u32 y, GPUFramebufferAttachmentType attachmentType) const;
    void enable(GPUFramebufferOperationType op);
    void disable(GPUFramebufferOperationType op);

private:
    u32 mFramebufferId = 0;
    std::vector<u32> mAttachmentIDs;
    GPUFramebufferData mFrameBufferData;

public:
    GET(FramebufferId)
};
