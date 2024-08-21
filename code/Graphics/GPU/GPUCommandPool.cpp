#include "GPUCommandPool.h"


#include "Core/Minimal.hpp"
//namespace GPUAPI {

    const VkAllocationCallbacks* GPUCommandPool::ALLOCATOR = VK_NULL_HANDLE;

    GPUCommandPool::GPUCommandPool(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice) : vulkanPhysicalDevice(vulkanPhysicalDevice), vulkanDevice(vulkanDevice) {
    }

    const VkCommandPool GPUCommandPool::getVkCommandPool() const {
        return vkCommandPool;
    }

    bool GPUCommandPool::initialize() {
        VkCommandPoolCreateInfo commandPoolInfo{};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = vulkanPhysicalDevice->getQueueFamilyIndices().GraphicsFamily.value();

        if (vkCreateCommandPool(vulkanDevice->getDevice(), &commandPoolInfo, ALLOCATOR, &vkCommandPool) != VK_SUCCESS) {
            CHECK_MSG(false,"Could not create Vulkan command pool");
            return false;
        }

        LOG("Created Vulkan command pool");
        return true;
    }

    void GPUCommandPool::terminate() {
        vkDestroyCommandPool(vulkanDevice->getDevice(), vkCommandPool, ALLOCATOR);
        LOG("Destroyed Vulkan command pool");
    }

    std::vector<GPUCommandBuffer> GPUCommandPool::allocateCommandBuffers(uint32_t count) const {
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

        std::vector<GPUCommandBuffer> vulkanCommandBuffers;
        for (VkCommandBuffer vkCommandBuffer : vkCommandBuffers) {
            GPUCommandBuffer vulkanCommandBuffer(vkCommandBuffer);
            vulkanCommandBuffers.push_back(vulkanCommandBuffer);
        }
        LOG("Allocated [{}] command buffers");
        return vulkanCommandBuffers;
    }

    void GPUCommandPool::free(const GPUCommandBuffer& commandBuffer) const {
        VkCommandBuffer vkCommandBuffer = commandBuffer.getVkCommandBuffer();
        vkFreeCommandBuffers(vulkanDevice->getDevice(), vkCommandPool, 1, &vkCommandBuffer);
    }

// }