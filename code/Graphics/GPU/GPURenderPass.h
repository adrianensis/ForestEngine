#pragma once

#include "Graphics/GPU/GPUProgramModule.h"
#include "Graphics/GPU/GPUSwapChain.h"
#include "Graphics/GPU/GPUDevice.h"
#include "Graphics/GPU/GPUCommandBuffer.h"

class GPUFramebuffer;

class GPURenderPass {
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPURenderPass(GPUSwapChain* vulkanSwapChain, GPUDevice* vulkanDevice, GPUPhysicalDevice* vulkanPhysicalDevice);
    const VkRenderPass getRenderPass() const;
    bool initialize();
    void terminate();
    void begin(const GPUCommandBuffer* vulkanCommandBuffer, const GPUFramebuffer& vulkanFramebuffer) const;
    void end(const GPUCommandBuffer* vulkanCommandBuffer) const;
private:
    VkFormat findDepthFormat();

private:
    GPUSwapChain* vulkanSwapChain;
    GPUDevice* vulkanDevice;
    GPUPhysicalDevice* vulkanPhysicalDevice;
    VkRenderPass renderPass = VK_NULL_HANDLE;
};



