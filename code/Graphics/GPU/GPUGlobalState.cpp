#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Graphics/Window/WindowManager.hpp"

void GPUGlobalState::init()
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

    vulkan = new Vulkan(config.mVulkan, GET_SYSTEM(WindowManager).getMainWindow().getInternalPointer());
    vulkanPhysicalDevice = new GPUPhysicalDevice(vulkan);
    vulkanDevice = new GPUDevice(vulkan, vulkanPhysicalDevice);
    vulkanSwapChain = new GPUSwapChain(vulkanDevice, vulkanPhysicalDevice, vulkan, GET_SYSTEM(WindowManager).getMainWindow().getInternalPointer());
    vulkanCommandPool = new GPUCommandPool(vulkanPhysicalDevice, vulkanDevice);
    
    if (!vulkan->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan");
    }
    if (!vulkanPhysicalDevice->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan physical device");
    }
    if (!vulkanDevice->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan device");
    }
    if (!vulkanSwapChain->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan swap chain");
    }
    if (!vulkanCommandPool->initialize()) 
    {
        CHECK_MSG(false, "Could not initialize Vulkan command pool");
    }
    vulkanCommandBuffers = vulkanCommandPool->allocateCommandBuffers(MAX_FRAMES_IN_FLIGHT);
    if (vulkanCommandBuffers.empty())
    {
        CHECK_MSG(false, "Could not initialize Vulkan command buffers");
    }
}

u32 GPUGlobalState::requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType)
{
    u32 bindingPoint = 0;
    switch (gpuUniformBufferType)
    {
    case GPUBufferType::UNIFORM:
        {
            bindingPoint = mBindingPointsIndexUniform;
            mBindingPointsIndexUniform++;
            CHECK_MSG((i32)mBindingPointsIndexUniform <= mMaxUniformBufferBindingPointsUniform, "Max Uniform Binding Points reached!");
        }
        break;
    case GPUBufferType::STORAGE:
        {
            bindingPoint = mBindingPointsIndexStorage;
            mBindingPointsIndexStorage++;
            CHECK_MSG((i32)mBindingPointsIndexStorage <= mMaxUniformBufferBindingPointsStorage, "Max Storage Binding Points reached!");
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
VkCommandBuffer GPUGlobalState::beginSingleTimeCommands() const {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vulkanCommandPool->getVkCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vulkanDevice->getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void GPUGlobalState::endSingleTimeCommands(VkCommandBuffer commandBuffer) const {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkFence fence = VK_NULL_HANDLE;
    constexpr uint32_t submitCount = 1;
    vkQueueSubmit(vulkanDevice->getGraphicsQueue(), submitCount, &submitInfo, fence);
    vkQueueWaitIdle(vulkanDevice->getGraphicsQueue());

    vkFreeCommandBuffers(vulkanDevice->getDevice(), vulkanCommandPool->getVkCommandPool(), submitInfo.commandBufferCount, &commandBuffer);
}


bool GPUGlobalState::initializeSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vulkanDevice->getDevice(), &semaphoreInfo, allocationCallbacks, &imageAvailableSemaphores[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'image available' semaphore for frame [{}]");
        }
        if (vkCreateSemaphore(vulkanDevice->getDevice(), &semaphoreInfo, allocationCallbacks, &renderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'render finished' semaphore for frame [{}]");
        }
        if (vkCreateFence(vulkanDevice->getDevice(), &fenceInfo, allocationCallbacks, &inFlightFences[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'in flight' fence for frame [{}]");
        }
    }
    LOG("Created Vulkan sync objects (semaphores & fences)");
    return true;
}

bool GPUGlobalState::hasStencilComponent(VkFormat format) const
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

bool GPUGlobalState::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) const {
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

VkImageView GPUGlobalState::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
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
    if (vkCreateImageView(vulkanDevice->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan image view");
        return nullptr;
    }
    return imageView;
}

void GPUGlobalState::terminate()
{
    mGPUUniformBuffersContainer.terminate();

    // terminateSyncObjects
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vulkanDevice->getDevice(), renderFinishedSemaphores[i], allocationCallbacks);
        vkDestroySemaphore(vulkanDevice->getDevice(), imageAvailableSemaphores[i], allocationCallbacks);
        vkDestroyFence(vulkanDevice->getDevice(), inFlightFences[i], allocationCallbacks);
    }
    LOG("Destroyed Vulkan sync objects (semaphores & fences)");

    vulkanSwapChain->terminate();
    delete vulkanSwapChain;
    delete vulkanDevice;
    delete vulkanPhysicalDevice;
    delete vulkanCommandPool;
    delete vulkan;
}