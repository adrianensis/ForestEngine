#pragma once

#include "Graphics/GPU/GPUShader.h"
#include "Graphics/GPU/GPURenderPass.h"
#include "Graphics/GPU/GPUSwapChain.h"
#include "Graphics/GPU/GPUDevice.h"

#include <vulkan/vulkan.h>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUGraphicsPipeline {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        GPURenderPass* vulkanRenderPass;
        GPUSwapChain* vulkanSwapChain;
        GPUDevice* vulkanDevice;
        GPUPhysicalDevice* vulkanPhysicalDevice;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;

    public:
        GPUGraphicsPipeline(GPURenderPass* vulkanRenderPass, GPUSwapChain* vulkanSwapChain, GPUDevice* vulkanDevice, GPUPhysicalDevice* vulkanPhysicalDevice);

        const VkPipelineLayout getPipelineLayout() const;

        const VkPipeline getPipeline() const;

        bool initialize(const GPUShader& vertexShader, const GPUShader& fragmentShader, VkDescriptorSetLayout descriptorSetLayout);

        void terminate();

        void bind(const GPUCommandBuffer& vulkanCommandBuffer) const;
    };

// }


