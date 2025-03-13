#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/Image/GPUImageUtils.hpp"

GPURenderPass::GPURenderPass(){}

bool GPURenderPass::init(Ptr<GPUContext> gpuContext, const GPURenderPassData& gpuRenderPassData)
{
    PROFILER_CPU()
    mGPUContext = gpuContext;
    mGPURenderPassData = gpuRenderPassData;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mGPUContext->vulkanSwapChain->getSurfaceFormat().format;
    colorAttachment.samples = mGPURenderPassData.mSampleCountFlagBits;
    colorAttachment.loadOp = mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
        (VkAttachmentLoadOp) mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp :
        VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
        (VkAttachmentStoreOp) mGPURenderPassData.mColorAttachment.mGPUAttachmentStoreOp :
        VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    if(mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp == GPUAttachmentLoadOp::LOAD)
    {
        colorAttachment.initialLayout = mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR :
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    else
    {
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    colorAttachment.finalLayout = mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR :
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = mGPURenderPassData.mSampleCountFlagBits;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // if(mGPURenderPassData.mDepthStencilAttachment.mGPUAttachmentLoadOp == GPUAttachmentLoadOp::LOAD)
    // {
    //     depthAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // }
    // else
    // {
    //     depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // }
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkSubpassDependency, 1> dependencies{};
    // std::array<VkSubpassDependency, 2> dependencies{};

    // Depth attachment
    // dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    // dependencies[0].dstSubpass = 0;
    // dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    // dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    // dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    // dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    // dependencies[0].dependencyFlags = 0;
    // Color attachment
    // dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    // dependencies[1].dstSubpass = 0;
    // dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // dependencies[1].srcAccessMask = 0;
    // dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    // dependencies[1].dependencyFlags = 0;
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

    std::vector<VkAttachmentDescription> attachments = {colorAttachment, depthAttachment};

    if(mGPURenderPassData.mSampleCountFlagBits > VK_SAMPLE_COUNT_1_BIT)
    {
        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = mGPUContext->vulkanSwapChain->getSurfaceFormat().format;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachments.push_back(colorAttachmentResolve);

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        subpass.pResolveAttachments = &colorAttachmentResolveRef;
    }

    // std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = (u32) attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

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
    colorImageConfig.SampleCount = mGPURenderPassData.mSampleCountFlagBits;
    colorImageConfig.Format = colorFormat;
    colorImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageConfig.Usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT /*| VK_IMAGE_USAGE_TRANSFER_DST_BIT*/;
    colorImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    colorImageConfig.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (!vulkanColorImage.init(mGPUContext, colorImageConfig)) {
        CHECK_MSG(false,"Could not initialize color image");
        return false;
    }
    colorImageView = GPUImageUtils::createImageView(mGPUContext, vulkanColorImage.getVkImage(), colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, colorImageConfig.MipLevels);
    
    // if(mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp == GPUAttachmentLoadOp::LOAD)
    // {
    //     GPUImageUtils::transitionImageLayout(mGPUContext, vulkanColorImage.getVkImage(), colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, colorImageConfig.MipLevels);
    // }
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
    depthImageConfig.SampleCount = mGPURenderPassData.mSampleCountFlagBits;
    depthImageConfig.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (!vulkanDepthImage.init(mGPUContext, depthImageConfig)) {
        CHECK_MSG(false,"Could not initialize depth image");
        return false;
    }
    depthImageView = GPUImageUtils::createImageView(mGPUContext, vulkanDepthImage.getVkImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, depthImageConfig.MipLevels);
    // GPUImageUtils::transitionImageLayout(mGPUContext, vulkanDepthImage.getVkImage(), depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, depthImageConfig.MipLevels);
    return true;
}

VkFormat GPURenderPass::findDepthFormat()
{
    std::vector<VkFormat> candidates = {
            VK_FORMAT_D32_SFLOAT_S8_UINT, // max priority for depth 32 bits stencil 8 bits 
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT
    };
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    return mGPUContext->vulkanPhysicalDevice->findSupportedFormat(candidates, tiling, features);
}

bool GPURenderPass::initializeFramebuffers()
{
    PROFILER_CPU()
    for (VkImageView swapChainImageView : mGPUContext->vulkanSwapChain->getImageViews())
    {
        std::vector<VkImageView> attachments;
        if(mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT)
        {
            attachments =
            {
                swapChainImageView,
                depthImageView,
            };
        }
        else
        {
            attachments =
            {
                colorImageView,
                depthImageView,
                swapChainImageView
            };
        }
        
        GPUFramebuffer framebuffer;
        if (!framebuffer.init(mGPUContext, GPUFramebufferData{}, this, attachments))
        {
            CHECK_MSG(false,"Could not initialize framebuffers");
            return false;
        }
        framebuffers.push_back(framebuffer);
    }
    LOG("Created [{}] Vulkan framebuffers", framebuffers.size());
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
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mRenderPass;
    renderPassInfo.framebuffer = framebuffers.at(mGPUContext->currentSwapChainImageIndex).getFramebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mGPUContext->vulkanSwapChain->getExtent();

    VkClearColorValue clearColorValue = {{0.1f, 0.2f, 0.1f, 1.0f}};

    // The range of depths in the depth buffer is 0.0 to 1.0 in Vulkan, where 1.0 lies at the far view plane and 0.0 at the near view plane.
    // The initial value at each point in the depth buffer should be the furthest possible depth, which is 1.0.
    VkClearDepthStencilValue clearDepthStencilValue{};
    clearDepthStencilValue.depth = 1.0f;
    clearDepthStencilValue.stencil = 0;

    // Note that the order of clearValues should be identical to the order of your attachments.
    std::vector<VkClearValue> clearValues;
    clearValues.resize(mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ? 2 : 3);
    
    clearValues[0].color = clearColorValue;
    clearValues[1].depthStencil = clearDepthStencilValue;

    if(mGPURenderPassData.mSampleCountFlagBits > VK_SAMPLE_COUNT_1_BIT)
    {
        clearValues[2].color = clearColorValue;
    }

    renderPassInfo.clearValueCount = (u32) clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    vkCmdBeginRenderPass(vulkanCommandBuffer.getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //set dynamic viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
    viewport.y = (float) mGPUContext->vulkanSwapChain->getExtent().height;
    viewport.width = (float) mGPUContext->vulkanSwapChain->getExtent().width;
    viewport.height = -(float) mGPUContext->vulkanSwapChain->getExtent().height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vkCmdSetViewport(vulkanCommandBuffer.getVkCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = mGPUContext->vulkanSwapChain->getExtent().width;
	scissor.extent.height = mGPUContext->vulkanSwapChain->getExtent().height;

	vkCmdSetScissor(vulkanCommandBuffer.getVkCommandBuffer(), 0, 1, &scissor);
}

void GPURenderPass::end()
{
    PROFILER_CPU()

    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    vkCmdEndRenderPass(vulkanCommandBuffer.getVkCommandBuffer());
}

void GPURenderPass::clearColor()
{
    PROFILER_CPU()
    VkClearColorValue clearColorValue = {{0.0f, 0.0f, 0.0f, 1.0f}};
    const VkImageSubresourceRange clear_range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    vkCmdClearColorImage(vulkanCommandBuffer.getVkCommandBuffer(), vulkanColorImage.getVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL/*VK_IMAGE_LAYOUT_GENERAL*/, &clearColorValue, 1, &clear_range);
}
void GPURenderPass::clearDepthStencil()
{
    PROFILER_CPU()
    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    VkClearDepthStencilValue clearDepthStencilValue{};
    clearDepthStencilValue.depth = 1.0f;
    clearDepthStencilValue.stencil = 0;
    const VkImageSubresourceRange clear_range = { VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1 };
    vkCmdClearDepthStencilImage(vulkanCommandBuffer.getVkCommandBuffer(), vulkanDepthImage.getVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL/*VK_IMAGE_LAYOUT_GENERAL*/, &clearDepthStencilValue, 1, &clear_range);
}

void GPURenderPass::onResize()
{
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
}