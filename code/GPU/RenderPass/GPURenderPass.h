#pragma once

#include "GPU/Shader/GPUShaderModule.h"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Core/GPUCommandBuffer.h"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/Image/GPUImage.h"

class GPURenderPass : public EnablePtrToThis
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPURenderPass(WeakPtr<GPUContext> gpuContext);
    bool init();
    void terminate();
    void begin();
    void end();
private:
    VkFormat findDepthFormat();
    bool initializeColorResources();
    bool initializeDepthResources();
    bool initializeFramebuffers();


public:
    WeakPtr<GPUContext> mGPUContext;
private:
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    u32 swapChainImageIndex;
    std::vector<GPUFramebuffer> framebuffers;
    GPUImage vulkanDepthImage;
    VkImageView depthImageView = VK_NULL_HANDLE;
    GPUImage vulkanColorImage;
    VkImageView colorImageView = VK_NULL_HANDLE;
public:
    CRGET(RenderPass)
};



