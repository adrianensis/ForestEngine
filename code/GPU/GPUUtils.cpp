#include "GPU/GPUUtils.hpp"

/*
    * TODO
    *
    * All of the helper functions that submit commands so far have been set up to execute synchronously by waiting for the queue to become idle.
    *
    * For practical applications it is recommended to combine these operations in a single command buffer and execute them asynchronously for higher throughput,
    * especially the transitions and copy in the createTextureImage function.
    *
    * Try to experiment with this by creating a setupCommandBuffer that the helper functions record commands into,
    * and add a flushSetupCommands to execute the commands that have been recorded so far.
    *
    * It's best to do this after the texture mapping works to check if the texture resources are still set up correctly.
    */
VkCommandBuffer GPUUtils::beginSingleTimeCommands(WeakPtr<GPUContext> gpuContext) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = gpuContext->vulkanCommandPool->getVkCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(gpuContext->vulkanDevice->getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void GPUUtils::endSingleTimeCommands(WeakPtr<GPUContext> gpuContext, VkCommandBuffer commandBuffer)
{
#if defined(ENGINE_ENABLE_PROFILER)
  PROFILER_GPU_COLLECT(gpuContext->mTracyContext, commandBuffer);
#endif

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkFence fence = VK_NULL_HANDLE;
    constexpr u32 submitCount = 1;
    vkQueueSubmit(gpuContext->vulkanDevice->getGraphicsQueue(), submitCount, &submitInfo, fence);
    vkQueueWaitIdle(gpuContext->vulkanDevice->getGraphicsQueue());

    vkFreeCommandBuffers(gpuContext->vulkanDevice->getDevice(), gpuContext->vulkanCommandPool->getVkCommandPool(), submitInfo.commandBufferCount, &commandBuffer);
}

bool GPUUtils::hasStencilComponent(WeakPtr<GPUContext> gpuContext, VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void GPUUtils::drawIndexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance)
{
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}


void GPUUtils::beginCmd(WeakPtr<GPUContext> gpuContext, VkRenderPass renderPass, VkFramebuffer vulkanFramebuffer)
{
    PROFILER_CPU()

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = vulkanFramebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = gpuContext->vulkanSwapChain->getExtent();

    VkClearColorValue clearColorValue = {
            {0.1f, 0.2f, 0.1f, 1.0f}
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

    renderPassInfo.clearValueCount = (u32) clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    const GPUCommandBuffer* vulkanCommandBuffer = gpuContext->vulkanCommandBuffers[gpuContext->currentFrame];
    vulkanCommandBuffer->reset();
    vulkanCommandBuffer->begin();


    vkCmdBeginRenderPass(vulkanCommandBuffer->getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void GPUUtils::endCmd(WeakPtr<GPUContext> gpuContext)
{
    PROFILER_CPU()
    const GPUCommandBuffer* vulkanCommandBuffer = gpuContext->vulkanCommandBuffers[gpuContext->currentFrame];
    vkCmdEndRenderPass(vulkanCommandBuffer->getVkCommandBuffer());

    if (!vulkanCommandBuffer->end()) {
        CHECK_MSG(false, "Could not end frame");
    }
}

u32 GPUUtils::frameAcquisition(WeakPtr<GPUContext> gpuContext)
{
    PROFILER_CPU()

    u32 swapChainImageIndex = 0;

    waitForFence(gpuContext, gpuContext->currentFrame);

    // Acquire an image from the swap chain
    VkFence acquireNextImageFence = VK_NULL_HANDLE;
    constexpr uint64_t acquireNextImageTimeout = UINT64_MAX;
    VkSemaphore imageAvailableSemaphore = gpuContext->imageAvailableSemaphores[gpuContext->currentFrame];
    VkResult acquireNextImageResult = vkAcquireNextImageKHR(
            gpuContext->vulkanDevice->getDevice(),
            gpuContext->vulkanSwapChain->getSwapChain(),
            acquireNextImageTimeout,
            imageAvailableSemaphore,
            acquireNextImageFence,
            &swapChainImageIndex
    );
    // VK_ERROR_OUT_OF_DATE_KHR: The swap chain has become incompatible with the surface and can no longer be used for rendering. Usually happens after a window resize.
    if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        //recreateRenderingObjects();
        CHECK_MSG(false, "TODO: implement window resize");
    }
    // VK_SUBOPTIMAL_KHR: The swap chain can still be used to successfully present to the surface, but the surface properties are no longer matched exactly.
    if (acquireNextImageResult != VK_SUCCESS && acquireNextImageResult != VK_SUBOPTIMAL_KHR)
    {
        CHECK_MSG(false, "Could not acquire swap chain image");
    }

    // After waiting, we need to manually reset the fence to the unsignaled state
    constexpr u32 fenceCount = 1;
    VkFence inFlightFence = gpuContext->inFlightFences[gpuContext->currentFrame];
    vkResetFences(gpuContext->vulkanDevice->getDevice(), fenceCount, &inFlightFence);

    return swapChainImageIndex;
}

void GPUUtils::waitForFence(WeakPtr<GPUContext> gpuContext, u32 frameIndex)
{
    // Wait until the previous frame has finished
    constexpr u32 fenceCount = 1;
    constexpr VkBool32 waitForAllFences = VK_TRUE;
    constexpr uint64_t waitForFenceTimeout = UINT64_MAX;
    VkFence inFlightFence = gpuContext->inFlightFences[frameIndex % GPUContext::MAX_FRAMES_IN_FLIGHT];
    VkResult waitResult = vkWaitForFences(gpuContext->vulkanDevice->getDevice(), fenceCount, &inFlightFence, waitForAllFences, waitForFenceTimeout);

}

void GPUUtils::commandSubmission(WeakPtr<GPUContext> gpuContext)
{
    PROFILER_CPU()
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const GPUCommandBuffer* vulkanCommandBuffer = gpuContext->vulkanCommandBuffers[gpuContext->currentFrame];
    VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer->getVkCommandBuffer();
    submitInfo.pCommandBuffers = &vkCommandBuffer;
    submitInfo.commandBufferCount = 1;

    // Wait with writing colors to the image until it's available
    VkSemaphore imageAvailableSemaphore = gpuContext->imageAvailableSemaphores[gpuContext->currentFrame];
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.waitSemaphoreCount = 1;

    // Which semaphores to signal once the command buffer(s) have finished execution
    VkSemaphore renderFinishedSemaphore = gpuContext->renderFinishedSemaphores[gpuContext->currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.signalSemaphoreCount = 1;

    // Submit recorded graphics commands
    constexpr u32 submitCount = 1;
    VkFence inFlightFence = gpuContext->inFlightFences[gpuContext->currentFrame];
    if (vkQueueSubmit(gpuContext->vulkanDevice->getGraphicsQueue(), submitCount, &submitInfo, inFlightFence) != VK_SUCCESS) {
        VULKAN_LOG_ERROR("Could not submit to graphics queue")
        CHECK_MSG(false, "Could not submit to graphics queue")
    }
}

void GPUUtils::framePresentation(WeakPtr<GPUContext> gpuContext, const std::vector<u32>& imageIndices)
{
    PROFILER_CPU()
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Which semaphores to wait on before presentation can happen
    VkSemaphore renderFinishedSemaphore = gpuContext->renderFinishedSemaphores[gpuContext->currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.waitSemaphoreCount = 1;

    // Which swap chain to present image to
    VkSwapchainKHR swapChains[] = {gpuContext->vulkanSwapChain->getSwapChain()};
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndices.data();
    presentInfo.swapchainCount = imageIndices.size();

    // Present image to swap chain
    {
        PROFILER_CPU_NAMED(vkQueuePresentKHR)
        VkResult presentResult = vkQueuePresentKHR(gpuContext->vulkanDevice->getPresentQueue(), &presentInfo);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR /*|| windowResized*/) {
            // windowResized = false;
            // recreateRenderingObjects();
        } else if (presentResult != VK_SUCCESS) {
            VULKAN_LOG_ERROR("Could not present image to swap chain")
            CHECK_MSG(false, "Could not present image to swap chain")
        }
    }
}
