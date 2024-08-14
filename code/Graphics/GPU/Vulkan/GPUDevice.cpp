#include "GPUDevice.h"
#include "Log.h"

#include <set>

namespace GPUAPI {

    const VkAllocationCallbacks* GPUDevice::ALLOCATOR = VK_NULL_HANDLE;

    GPUDevice::GPUDevice(Vulkan* vulkan, GPUPhysicalDevice* vulkanPhysicalDevice) : vulkan(vulkan), vulkanPhysicalDevice(vulkanPhysicalDevice) {
    }

    const VkDevice GPUDevice::getDevice() const {
        return device;
    }

    const VkQueue GPUDevice::getGraphicsQueue() const {
        return graphicsQueue;
    }

    const VkQueue GPUDevice::getPresentQueue() const {
        return presentQueue;
    }

    bool GPUDevice::initialize() {
        const QueueFamilyIndices& queueFamilyIndices = vulkanPhysicalDevice->getQueueFamilyIndices();
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = getDeviceQueueCreateInfos(queueFamilyIndices);
        if (queueCreateInfos.empty()) {
            VD_LOG_ERROR("Could not get any device queue create infos");
            return false;
        }
        if (!createDevice(queueCreateInfos)) {
            VD_LOG_ERROR("Could not create Vulkan device");
            return false;
        }
        VD_LOG_INFO("Created Vulkan device");

        if (!findDeviceQueues(queueFamilyIndices)) {
            VD_LOG_ERROR("Could not find any Vulkan device queues");
            return false;
        }
        VD_LOG_INFO("Found Vulkan queues");
        return true;
    }

    void GPUDevice::terminate() const {
        vkDestroyDevice(device, ALLOCATOR);
        VD_LOG_INFO("Destroyed Vulkan device");
    }

    void GPUDevice::waitUntilIdle() const {
        vkDeviceWaitIdle(device);
    }

    std::vector<VkDeviceQueueCreateInfo> GPUDevice::getDeviceQueueCreateInfos(const QueueFamilyIndices& queueFamilyIndices) const {
        constexpr float queuePriority = 1.0f;
        std::set<uint32_t> queueFamilies = {
                queueFamilyIndices.GraphicsFamily.value(),
                queueFamilyIndices.PresentationFamily.value()
        };
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (uint32_t queueFamily : queueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfo.queueCount = 1;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        return queueCreateInfos;
    }

    bool GPUDevice::createDevice(const std::vector<VkDeviceQueueCreateInfo>& deviceQueueCreateInfos) {
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pEnabledFeatures = &vulkanPhysicalDevice->getFeatures();
        createInfo.enabledExtensionCount = vulkanPhysicalDevice->getExtensions().size();
        createInfo.ppEnabledExtensionNames = vulkanPhysicalDevice->getExtensions().data();
        createInfo.queueCreateInfoCount = deviceQueueCreateInfos.size();
        createInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
        if (vulkan->isValidationLayersEnabled()) {
            createInfo.enabledLayerCount = vulkan->getValidationLayers().size();
            createInfo.ppEnabledLayerNames = vulkan->getValidationLayers().data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        return vkCreateDevice(vulkanPhysicalDevice->getPhysicalDevice(), &createInfo, ALLOCATOR, &device) == VK_SUCCESS;
    }

    bool GPUDevice::findDeviceQueues(const QueueFamilyIndices& queueFamilyIndices) {
        graphicsQueue = findDeviceQueue(queueFamilyIndices.GraphicsFamily.value());
        if (graphicsQueue == VK_NULL_HANDLE) {
            VD_LOG_ERROR("Could not get Vulkan graphics queue");
            return false;
        }
        presentQueue = findDeviceQueue(queueFamilyIndices.PresentationFamily.value());
        if (presentQueue == VK_NULL_HANDLE) {
            VD_LOG_ERROR("Could not get Vulkan present queue");
            return false;
        }
        return true;
    }

    VkQueue GPUDevice::findDeviceQueue(uint32_t queueFamilyIndex) const {
        constexpr int queueIndex = 0;
        VkQueue queue;
        vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);
        return queue;
    }

}