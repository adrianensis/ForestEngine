#pragma once

#include "GPUSwapChain.h"
#include "GPURenderPass.h"

#include <vulkan/vulkan.h>

namespace GPUAPI {

    class GPUFramebuffer {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        GPUDevice* vulkanDevice;
        GPUSwapChain* vulkanSwapChain;
        GPURenderPass* vulkanRenderPass;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;

    public:
        GPUFramebuffer(GPUDevice* vulkanDevice, GPUSwapChain* vulkanSwapChain, GPURenderPass* vulkanRenderPass);

        const VkFramebuffer getFramebuffer() const;

        bool initialize(VkImageView colorImageView, VkImageView depthImageView, VkImageView swapChainImageView);

        void terminate();
    };

}
