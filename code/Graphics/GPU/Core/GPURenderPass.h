#pragma once

#include "Graphics/GPU/Shader/GPUShaderModule.h"
#include "Graphics/GPU/Core/GPUContext.hpp"
#include "Graphics/GPU/Core/GPUCommandBuffer.h"

class GPUFramebuffer;

class GPURenderPass {
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPURenderPass(Ptr<GPUContext> gpuContext);
    const VkRenderPass getRenderPass() const;
    bool initialize();
    void terminate();
    void begin(const GPUCommandBuffer* vulkanCommandBuffer, const GPUFramebuffer& vulkanFramebuffer) const;
    void end(const GPUCommandBuffer* vulkanCommandBuffer) const;
private:
    VkFormat findDepthFormat();

private:
    Ptr<GPUContext> mGPUContext;
    VkRenderPass renderPass = VK_NULL_HANDLE;
};


