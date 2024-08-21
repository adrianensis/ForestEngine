#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/GPU/GPUPhysicalDevice.h"
#include "Graphics/GPU/GPUDevice.h"
#include "Graphics/GPU/GPUSwapChain.h"
#include "Graphics/GPU/GPUCommandPool.h"
#include "Graphics/GPU/GPUCommandBuffer.h"

void GPUInstance::init()
{
//    mMaxUniformBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::UNIFORM);
//    mMaxUniformBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::STORAGE);

    Config config{};
    config.Name = "GPUAPI";
    // config.LogLevel = Log::Level::Debug;
    // config.mWindow.Title = config.Name;
    // config.mWindow.Width = 800;
    // config.mWindow.Height = 600;
    config.mVulkan.Name = config.Name;

    mGPUContext = OwnerPtr<GPUContext>::newObject();
    mGPUContext->init();
}

u32 GPUInstance::requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType)
{
    u32 bindingPoint = 0;
    switch (gpuUniformBufferType)
    {
    case GPUBufferType::UNIFORM:
        {
            bindingPoint = mBindingPointsIndexUniform;
            mBindingPointsIndexUniform++;
            // CHECK_MSG((i32)mBindingPointsIndexUniform <= mMaxUniformBufferBindingPointsUniform, "Max Uniform Binding Points reached!");
        }
        break;
    case GPUBufferType::STORAGE:
        {
            bindingPoint = mBindingPointsIndexStorage;
            mBindingPointsIndexStorage++;
            // CHECK_MSG((i32)mBindingPointsIndexStorage <= mMaxUniformBufferBindingPointsStorage, "Max Storage Binding Points reached!");
        }
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    return bindingPoint;
}



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
VkCommandBuffer GPUInstance::beginSingleTimeCommands() const {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mGPUContext->vulkanCommandPool->getVkCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(mGPUContext->vulkanDevice->getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void GPUInstance::endSingleTimeCommands(VkCommandBuffer commandBuffer) const {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkFence fence = VK_NULL_HANDLE;
    constexpr uint32_t submitCount = 1;
    vkQueueSubmit(mGPUContext->vulkanDevice->getGraphicsQueue(), submitCount, &submitInfo, fence);
    vkQueueWaitIdle(mGPUContext->vulkanDevice->getGraphicsQueue());

    vkFreeCommandBuffers(mGPUContext->vulkanDevice->getDevice(), mGPUContext->vulkanCommandPool->getVkCommandPool(), submitInfo.commandBufferCount, &commandBuffer);
}


bool GPUInstance::initializeSyncObjects() {
    mGPUContext->imageAvailableSemaphores.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    mGPUContext->renderFinishedSemaphores.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    mGPUContext->inFlightFences.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(mGPUContext->vulkanDevice->getDevice(), &semaphoreInfo, allocationCallbacks, &mGPUContext->imageAvailableSemaphores[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'image available' semaphore for frame [{}]");
        }
        if (vkCreateSemaphore(mGPUContext->vulkanDevice->getDevice(), &semaphoreInfo, allocationCallbacks, &mGPUContext->renderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'render finished' semaphore for frame [{}]");
        }
        if (vkCreateFence(mGPUContext->vulkanDevice->getDevice(), &fenceInfo, allocationCallbacks, &mGPUContext->inFlightFences[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'in flight' fence for frame [{}]");
        }
    }
    LOG("Created Vulkan sync objects (semaphores & fences)");
    return true;
}

bool GPUInstance::hasStencilComponent(VkFormat format) const
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

bool GPUInstance::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) const {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = oldLayout;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = 0;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (hasStencilComponent(format)) {
            imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        CHECK_MSG(false,"Could not transition image layout: Unsupported transition");
        return false;
    }

    constexpr VkDependencyFlags dependencyFlags = 0;
    constexpr uint32_t memoryBarrierCount = 0;
    constexpr VkMemoryBarrier* memoryBarriers = VK_NULL_HANDLE;
    constexpr uint32_t bufferMemoryBarrierCount = 0;
    constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = VK_NULL_HANDLE;
    constexpr uint32_t imageMemoryBarrierCount = 1;
    vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage,
            destinationStage,
            dependencyFlags,
            memoryBarrierCount,
            memoryBarriers,
            bufferMemoryBarrierCount,
            bufferMemoryBarriers,
            imageMemoryBarrierCount,
            &imageMemoryBarrier
    );

    endSingleTimeCommands(commandBuffer);
    return true;
}

VkImageView GPUInstance::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(mGPUContext->vulkanDevice->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan image view");
        return nullptr;
    }
    return imageView;
}

void GPUInstance::terminate()
{
    mGPUUniformBuffersContainer.terminate();

    // terminateSyncObjects
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(mGPUContext->vulkanDevice->getDevice(), mGPUContext->renderFinishedSemaphores[i], allocationCallbacks);
        vkDestroySemaphore(mGPUContext->vulkanDevice->getDevice(), mGPUContext->imageAvailableSemaphores[i], allocationCallbacks);
        vkDestroyFence(mGPUContext->vulkanDevice->getDevice(), mGPUContext->inFlightFences[i], allocationCallbacks);
    }
    LOG("Destroyed Vulkan sync objects (semaphores & fences)");

    mGPUContext->vulkanSwapChain->terminate();
    delete mGPUContext->vulkanSwapChain;
    delete mGPUContext->vulkanDevice;
    delete mGPUContext->vulkanPhysicalDevice;
    delete mGPUContext->vulkanCommandPool;
    delete mGPUContext->vulkan;
}