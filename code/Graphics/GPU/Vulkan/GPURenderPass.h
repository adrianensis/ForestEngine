#pragma once

#include "GPUShader.h"
#include "GPUSwapChain.h"
#include "GPUDevice.h"
#include "GPUCommandBuffer.h"

#include <vulkan/vulkan.h>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUFramebuffer;

    class GPURenderPass {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        GPUSwapChain* vulkanSwapChain;
        GPUDevice* vulkanDevice;
        GPUPhysicalDevice* vulkanPhysicalDevice;
        VkRenderPass renderPass = VK_NULL_HANDLE;

    public:
        GPURenderPass(GPUSwapChain* vulkanSwapChain, GPUDevice* vulkanDevice, GPUPhysicalDevice* vulkanPhysicalDevice);

        const VkRenderPass getRenderPass() const;

        bool initialize();

        void terminate();

        void begin(const GPUCommandBuffer& vulkanCommandBuffer, const GPUFramebuffer& vulkanFramebuffer) const;

        void end(const GPUCommandBuffer& vulkanCommandBuffer) const;

    private:
        VkFormat findDepthFormat();
    };

// }



