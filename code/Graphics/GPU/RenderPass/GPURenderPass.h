#pragma once

#include "Graphics/GPU/Shader/GPUShaderModule.h"
#include "Graphics/GPU/Core/GPUContext.hpp"
#include "Graphics/GPU/Core/GPUCommandBuffer.h"
#include "Graphics/GPU/Framebuffer/GPUFramebuffer.hpp"
#include "Graphics/GPU/Image/GPUImage.h"

class GPURenderPass {
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPURenderPass(Ptr<GPUContext> gpuContext);
    bool initialize();
    void terminate();
    void begin();
    void end();
private:
    VkFormat findDepthFormat();
    bool initializeColorResources();
    bool initializeDepthResources();
    bool initializeFramebuffers();

    void beginCmd(const GPUCommandBuffer* vulkanCommandBuffer, const GPUFramebuffer& vulkanFramebuffer) const;
    void endCmd(const GPUCommandBuffer* vulkanCommandBuffer) const;
    void commandRecordingBegin();
    void commandRecordingEnd();
    void frameAcquisition();
    void commandSubmission();
    void framePresentation();
private:
    Ptr<GPUContext> mGPUContext;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    uint32_t swapChainImageIndex;
    std::vector<GPUFramebuffer> framebuffers;
    GPUImage* vulkanDepthImage;
    VkImageView depthImageView = VK_NULL_HANDLE;
    GPUImage* vulkanColorImage;
    VkImageView colorImageView = VK_NULL_HANDLE;
public:
    CRGET(RenderPass)
};



