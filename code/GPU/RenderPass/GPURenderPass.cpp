#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/Image/GPUImageUtils.hpp"

GPURenderPass::GPURenderPass(){}

bool GPURenderPass::init(Ptr<GPUContext> gpuContext, const GPURenderPassData& gpuRenderPassData)
{
    PROFILER_CPU()
    mGPUContext = gpuContext;
    mGPURenderPassData = gpuRenderPassData;

    // if(mGPURenderPassData.mIsResolvePass)
    // {
    //     CHECK_MSG(mGPURenderPassData.mSampleCountFlagBits > VK_SAMPLE_COUNT_1_BIT, "Resolve pass must be SAMPLE BIT > 1.")
    // }

    VkAttachmentDescription colorAttachment{};
    // TODO: colorAttachment.format must come from Output Framebuffer config
    colorAttachment.format = mGPUContext->vulkanSwapChain->getSurfaceFormat().format;
    colorAttachment.samples = mGPURenderPassData.mSampleCountFlagBits;
    colorAttachment.loadOp = mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
        (VkAttachmentLoadOp) mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp :
        VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = /*mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
        (VkAttachmentStoreOp) mGPURenderPassData.mColorAttachment.mGPUAttachmentStoreOp :*/
        VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if(!mGPURenderPassData.mIsResolvePass && mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp == GPUAttachmentLoadOp::LOAD)
    {
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        // colorAttachment.initialLayout = mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
        //     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR :
        //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    else
    {
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    if(mGPURenderPassData.mIsResolvePass && mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp == GPUAttachmentLoadOp::LOAD)
    {
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        // colorAttachment.initialLayout = mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
        //     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR :
        //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    // colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // colorAttachment.finalLayout = mGPURenderPassData.mSampleCountFlagBits == VK_SAMPLE_COUNT_1_BIT ?
    //     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR :
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if(mGPURenderPassData.mIsResolvePass)
    {
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    else
    {
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkAttachmentDescription depthAttachment{};
    // TODO: depthAttachment.format must come from Output Framebuffer config
    depthAttachment.format = GPUImageUtils::findDepthFormat(mGPUContext);
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

    // Color attachment
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    dependencies[0].dependencyFlags = 0;
    // Depth attachment
    dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    dependencies[1].dependencyFlags = 0;

    std::vector<VkAttachmentDescription> attachments = {colorAttachment, depthAttachment};

    if(mGPURenderPassData.mSampleCountFlagBits > VK_SAMPLE_COUNT_1_BIT && mGPURenderPassData.mIsResolvePass)
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

    if (!initializeFramebuffers())
    {
        CHECK_MSG(false, "Could not initialize Vulkan framebuffers");
    }

    return true;
}

bool GPURenderPass::initializeFramebuffers()
{
    PROFILER_CPU()

    if(mGPURenderPassData.mIsResolvePass)
    {
        FOR_RANGE(i, 0, mGPUContext->vulkanSwapChain->getImageViews().size())
        {
            GPUFramebuffer framebuffer;
            GPUFramebufferData gpuFramebufferData;
            gpuFramebufferData.mIsResolveFramebuffer = mGPURenderPassData.mIsResolvePass;
            gpuFramebufferData.mSwapchainIndex = i;
            gpuFramebufferData.mColorImage = mGPURenderPassData.mColorAttachment.mGPUImage;
            if (!framebuffer.init(mGPUContext, gpuFramebufferData, this))
            {
                CHECK_MSG(false,"Could not initialize framebuffers");
                return false;
            }
            framebuffers.push_back(framebuffer);
        }
        LOG("Created [{}] Vulkan framebuffers", framebuffers.size());
    }
    else
    {
        GPUFramebufferData gpuFramebufferData;
        gpuFramebufferData.mColorImage = mGPURenderPassData.mColorAttachment.mGPUImage;
        mOutputGPUFramebuffer.init(mGPUContext, gpuFramebufferData, this);
        framebuffers.push_back(mOutputGPUFramebuffer);
    }

    return true;
}

void GPURenderPass::terminate()
{
    PROFILER_CPU()

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
    if(mGPURenderPassData.mIsResolvePass)
    {
        renderPassInfo.framebuffer = framebuffers.at(mGPUContext->currentSwapChainImageIndex).getFramebuffer();
    }
    else
    {
        renderPassInfo.framebuffer = mOutputGPUFramebuffer.getFramebuffer();
    }
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

void GPURenderPass::onResize()
{
    for (GPUFramebuffer framebuffer : framebuffers) {
        framebuffer.terminate();
    }
    framebuffers.clear();
    LOG("Destroyed Vulkan framebuffers");

    if (!initializeFramebuffers())
    {
        CHECK_MSG(false, "Could not initialize Vulkan framebuffers");
    }
}