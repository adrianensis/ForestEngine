#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/Image/GPUImageUtils.hpp"

GPURenderPass::GPURenderPass(Ptr<GPUContext> gpuContext)
        : mGPUContext(gpuContext){}

bool GPURenderPass::init()
{
    PROFILER_CPU()
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mGPUContext->vulkanSwapChain->getSurfaceFormat().format;
    colorAttachment.samples = mGPUContext->vulkanDevice->getPhysicalDevice()->getSampleCount();
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = mGPUContext->vulkanDevice->getPhysicalDevice()->getSampleCount();
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = mGPUContext->vulkanSwapChain->getSurfaceFormat().format;
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
    renderPassInfo.attachmentCount = (u32) attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(mGPUContext->vulkanDevice->getDevice(), &renderPassInfo, ALLOCATOR, &mRenderPass) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan render pass");
        return false;
    }
    LOG("Created Vulkan render pass");

    if (!initializeColorResources())
    {
        CHECK_MSG(false, "Could not initialize color resources");
    }
    if (!initializeDepthResources())
    {
        CHECK_MSG(false, "Could not initialize depth resources");
    }
    if (!initializeFramebuffers())
    {
        CHECK_MSG(false, "Could not initialize Vulkan framebuffers");
    }

    return true;
}

bool GPURenderPass::initializeColorResources()
{
    PROFILER_CPU()

    VkFormat colorFormat = mGPUContext->vulkanSwapChain->getSurfaceFormat().format;

    GPUImageData colorImageConfig{};
    colorImageConfig.Width = mGPUContext->vulkanSwapChain->getExtent().width;
    colorImageConfig.Height = mGPUContext->vulkanSwapChain->getExtent().height;
    colorImageConfig.MipLevels = 1;
    colorImageConfig.SampleCount = mGPUContext->vulkanPhysicalDevice->getSampleCount();
    colorImageConfig.Format = colorFormat;
    colorImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageConfig.Usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    colorImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (!vulkanColorImage.init(mGPUContext, colorImageConfig)) {
        CHECK_MSG(false,"Could not initialize color image");
        return false;
    }
    colorImageView = GPUImageUtils::createImageView(mGPUContext, vulkanColorImage.getVkImage(), colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, colorImageConfig.MipLevels);
    return true;
}

bool GPURenderPass::initializeDepthResources()
{
    PROFILER_CPU()

    VkFormat depthFormat = findDepthFormat();

    GPUImageData depthImageConfig{};
    depthImageConfig.Width = mGPUContext->vulkanSwapChain->getExtent().width;
    depthImageConfig.Height = mGPUContext->vulkanSwapChain->getExtent().height;
    depthImageConfig.Format = depthFormat;
    depthImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImageConfig.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    depthImageConfig.MipLevels = 1;
    depthImageConfig.SampleCount = mGPUContext->vulkanPhysicalDevice->getSampleCount();

    if (!vulkanDepthImage.init(mGPUContext, depthImageConfig)) {
        CHECK_MSG(false,"Could not initialize depth image");
        return false;
    }
    depthImageView = GPUImageUtils::createImageView(mGPUContext, vulkanDepthImage.getVkImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, depthImageConfig.MipLevels);
    GPUImageUtils::transitionImageLayout(mGPUContext, vulkanDepthImage.getVkImage(), depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, depthImageConfig.MipLevels);
    return true;
}

VkFormat GPURenderPass::findDepthFormat()
{
    std::vector<VkFormat> candidates = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
    };
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    return mGPUContext->vulkanPhysicalDevice->findSupportedFormat(candidates, tiling, features);
}

bool GPURenderPass::initializeFramebuffers()
{
    PROFILER_CPU()
    for (VkImageView swapChainImageView : mGPUContext->vulkanSwapChain->getImageViews()) {
        GPUFramebuffer framebuffer;//(mGPUContext->vulkanDevice, mGPUContext->vulkanSwapChain, vulkanRenderPass);
        if (!framebuffer.init(mGPUContext, GPUFramebufferData{}, this, colorImageView, depthImageView, swapChainImageView)) {
            CHECK_MSG(false,"Could not initialize framebuffers");
            return false;
        }
        framebuffers.push_back(framebuffer);
    }
    LOG("Created [{}] Vulkan framebuffers");
    return true;
}

void GPURenderPass::terminate()
{
    PROFILER_CPU()

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), colorImageView, allocationCallbacks);
    vulkanColorImage.terminate();
    vkDestroyImageView(mGPUContext->vulkanDevice->getDevice(), depthImageView, allocationCallbacks);
    vulkanDepthImage.terminate();
    for (GPUFramebuffer framebuffer : framebuffers) {
        framebuffer.terminate();
    }
    framebuffers.clear();
    LOG("Destroyed Vulkan framebuffers");

    vkDestroyRenderPass(mGPUContext->vulkanDevice->getDevice(), mRenderPass, ALLOCATOR);
    LOG("Destroyed Vulkan render pass");
}
void GPURenderPass::begin()
{
    PROFILER_CPU()
    swapChainImageIndex = mGPUContext->frameAcquisition();
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mRenderPass;
    renderPassInfo.framebuffer = framebuffers.at(swapChainImageIndex).getFramebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mGPUContext->vulkanSwapChain->getExtent();

    VkClearColorValue clearColorValue = {{0.1f, 0.2f, 0.1f, 1.0f}};

    // The range of depths in the depth buffer is 0.0 to 1.0 in Vulkan, where 1.0 lies at the far view plane and 0.0 at the near view plane.
    // The initial value at each point in the depth buffer should be the furthest possible depth, which is 1.0.
    VkClearDepthStencilValue clearDepthStencilValue{};
    clearDepthStencilValue.depth = 1.0f;
    clearDepthStencilValue.stencil = 0;

    // Note that the order of clearValues should be identical to the order of your attachments.
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = clearColorValue;
    clearValues[1].depthStencil = clearDepthStencilValue;

    renderPassInfo.clearValueCount = (u32) clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    const GPUCommandBuffer* vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    vulkanCommandBuffer->reset();
    vulkanCommandBuffer->begin();

    vkCmdBeginRenderPass(vulkanCommandBuffer->getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void GPURenderPass::end()
{
    PROFILER_CPU()

    const GPUCommandBuffer* vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    vkCmdEndRenderPass(vulkanCommandBuffer->getVkCommandBuffer());

    if (!vulkanCommandBuffer->end()) {
        CHECK_MSG(false, "Could not end frame");
    }

    mGPUContext->commandSubmission();
    mGPUContext->framePresentation({swapChainImageIndex});
}