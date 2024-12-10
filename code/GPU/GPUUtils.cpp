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
VkCommandBuffer GPUUtils::beginSingleTimeCommands(Ptr<GPUContext> gpuContext) {
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

void GPUUtils::endSingleTimeCommands(Ptr<GPUContext> gpuContext, VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkFence fence = VK_NULL_HANDLE;
    constexpr uint32_t submitCount = 1;
    vkQueueSubmit(gpuContext->vulkanDevice->getGraphicsQueue(), submitCount, &submitInfo, fence);
    vkQueueWaitIdle(gpuContext->vulkanDevice->getGraphicsQueue());

    vkFreeCommandBuffers(gpuContext->vulkanDevice->getDevice(), gpuContext->vulkanCommandPool->getVkCommandPool(), submitInfo.commandBufferCount, &commandBuffer);
}


bool GPUUtils::initializeSyncObjects(Ptr<GPUContext> gpuContext) {
    gpuContext->imageAvailableSemaphores.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    gpuContext->renderFinishedSemaphores.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    gpuContext->inFlightFences.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(gpuContext->vulkanDevice->getDevice(), &semaphoreInfo, allocationCallbacks, &gpuContext->imageAvailableSemaphores[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'image available' semaphore for frame [{}]");
        }
        if (vkCreateSemaphore(gpuContext->vulkanDevice->getDevice(), &semaphoreInfo, allocationCallbacks, &gpuContext->renderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'render finished' semaphore for frame [{}]");
        }
        if (vkCreateFence(gpuContext->vulkanDevice->getDevice(), &fenceInfo, allocationCallbacks, &gpuContext->inFlightFences[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'in flight' fence for frame [{}]");
        }
    }
    LOG("Created Vulkan sync objects (semaphores & fences)");
    return true;
}

bool GPUUtils::hasStencilComponent(Ptr<GPUContext> gpuContext, VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void GPUUtils::drawIndexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance)
{
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}