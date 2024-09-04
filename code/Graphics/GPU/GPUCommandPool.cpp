#include "GPUCommandPool.h"
#include "Graphics/GPU/GPUCommandBuffer.h"

GPUCommandPool::GPUCommandPool(GPUDevice* vulkanDevice) : vulkanDevice(vulkanDevice) {
}

const VkCommandPool GPUCommandPool::getVkCommandPool() const {
    return vkCommandPool;
}

bool GPUCommandPool::initialize() {
    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = vulkanDevice->getPhysicalDevice()->getQueueFamilyIndices().GraphicsFamily.value();

    if (vkCreateCommandPool(vulkanDevice->getDevice(), &commandPoolInfo, ALLOCATOR, &vkCommandPool) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan command pool");
        return false;
    }

    VULKAN_LOG("Created Vulkan command pool");
    return true;
}

void GPUCommandPool::terminate() {
    vkDestroyCommandPool(vulkanDevice->getDevice(), vkCommandPool, ALLOCATOR);
    VULKAN_LOG("Destroyed Vulkan command pool");
}

std::vector<GPUCommandBuffer*> GPUCommandPool::allocateCommandBuffers(uint32_t count) const {
    std::vector<VkCommandBuffer> vkCommandBuffers;
    vkCommandBuffers.resize(count);

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = vkCommandBuffers.size();
    allocateInfo.commandPool = vkCommandPool;

    if (vkAllocateCommandBuffers(vulkanDevice->getDevice(), &allocateInfo, vkCommandBuffers.data()) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not allocate [{}] Vulkan command buffers");
        return {};
    }

    std::vector<GPUCommandBuffer*> vulkanCommandBuffers;
    for (VkCommandBuffer vkCommandBuffer : vkCommandBuffers) {
        GPUCommandBuffer* vulkanCommandBuffer = new GPUCommandBuffer();
        vulkanCommandBuffer->init(vkCommandBuffer);
        vulkanCommandBuffers.push_back(vulkanCommandBuffer);
    }
    VULKAN_LOG("Allocated [{}] command buffers");
    return vulkanCommandBuffers;
}

void GPUCommandPool::free(const GPUCommandBuffer* commandBuffer) const {
    VkCommandBuffer vkCommandBuffer = commandBuffer->getVkCommandBuffer();
    vkFreeCommandBuffers(vulkanDevice->getDevice(), vkCommandPool, 1, &vkCommandBuffer);
}