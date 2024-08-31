#include "Graphics/GPU/GPURenderPass.h"
#include "Graphics/GPU/GPUFramebuffer.hpp"

const VkAllocationCallbacks* GPURenderPass::ALLOCATOR = VK_NULL_HANDLE;

GPURenderPass::GPURenderPass(GPUSwapChain* vulkanSwapChain, GPUDevice* vulkanDevice, GPUPhysicalDevice* vulkanPhysicalDevice)
        : vulkanSwapChain(vulkanSwapChain), vulkanDevice(vulkanDevice), vulkanPhysicalDevice(vulkanPhysicalDevice) {}

const VkRenderPass GPURenderPass::getRenderPass() const {
    return renderPass;
}

bool GPURenderPass::initialize() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = vulkanSwapChain->getSurfaceFormat().format;
    colorAttachment.samples = vulkanPhysicalDevice->getSampleCount();
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = vulkanPhysicalDevice->getSampleCount();
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = vulkanSwapChain->getSurfaceFormat().format;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = (uint32_t) attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(vulkanDevice->getDevice(), &renderPassInfo, ALLOCATOR, &renderPass) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan render pass");
        return false;
    }
    LOG("Created Vulkan render pass");
    return true;
}

void GPURenderPass::terminate() {
    vkDestroyRenderPass(vulkanDevice->getDevice(), renderPass, ALLOCATOR);
    LOG("Destroyed Vulkan render pass");
}

void GPURenderPass::begin(const GPUCommandBuffer& vulkanCommandBuffer, const GPUFramebuffer& vulkanFramebuffer) const {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = vulkanFramebuffer.getFramebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vulkanSwapChain->getExtent();

    VkClearColorValue clearColorValue = {
            {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // The range of depths in the depth buffer is 0.0 to 1.0 in Vulkan, where 1.0 lies at the far view plane and 0.0 at the near view plane.
    // The initial value at each point in the depth buffer should be the furthest possible depth, which is 1.0.
    VkClearDepthStencilValue clearDepthStencilValue{};
    clearDepthStencilValue.depth = 1.0f;
    clearDepthStencilValue.stencil = 0;

    // Note that the order of clearValues should be identical to the order of your attachments.
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = clearColorValue;
    clearValues[1].depthStencil = clearDepthStencilValue;

    renderPassInfo.clearValueCount = (uint32_t) clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(vulkanCommandBuffer.getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void GPURenderPass::end(const GPUCommandBuffer& vulkanCommandBuffer) const {
    vkCmdEndRenderPass(vulkanCommandBuffer.getVkCommandBuffer());
}

VkFormat GPURenderPass::findDepthFormat() {
    std::vector<VkFormat> candidates = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
    };
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    return vulkanPhysicalDevice->findSupportedFormat(candidates, tiling, features);
}