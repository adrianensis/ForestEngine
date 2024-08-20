#include "GPUFramebuffer.h"


#include "Core/Minimal.hpp"
//namespace GPUAPI {

    const VkAllocationCallbacks* GPUFramebuffer::ALLOCATOR = VK_NULL_HANDLE;

    GPUFramebuffer::GPUFramebuffer(GPUDevice* vulkanDevice, GPUSwapChain* vulkanSwapChain, GPURenderPass* vulkanRenderPass)
            : vulkanDevice(vulkanDevice), vulkanSwapChain(vulkanSwapChain), vulkanRenderPass(vulkanRenderPass) {
    }

    const VkFramebuffer GPUFramebuffer::getFramebuffer() const {
        return framebuffer;
    }

    bool GPUFramebuffer::initialize(VkImageView colorImageView, VkImageView depthImageView, VkImageView swapChainImageView) {
        std::array<VkImageView, 3> attachments[] = {
                colorImageView,
                depthImageView,
                swapChainImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vulkanRenderPass->getRenderPass();
        framebufferInfo.attachmentCount = (uint32_t) attachments->size();
        framebufferInfo.pAttachments = attachments->data();
        framebufferInfo.width = vulkanSwapChain->getExtent().width;
        framebufferInfo.height = vulkanSwapChain->getExtent().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vulkanDevice->getDevice(), &framebufferInfo, ALLOCATOR, &framebuffer) != VK_SUCCESS) {
            CHECK_MSG(false,"Could not create Vulkan framebuffer");
            return false;
        }
        return true;
    }

    void GPUFramebuffer::terminate() {
        vkDestroyFramebuffer(vulkanDevice->getDevice(), framebuffer, ALLOCATOR);
    }

// }
