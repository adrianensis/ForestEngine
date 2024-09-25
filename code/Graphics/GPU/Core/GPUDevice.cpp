#include "GPUDevice.h"


GPUDevice::GPUDevice(Vulkan* vulkan, GPUPhysicalDevice* vulkanPhysicalDevice) :
    mVulkan(vulkan), mPhysicalDevice(vulkanPhysicalDevice) {}

bool GPUDevice::init()
{
    const GPUQueueFamilyIndices& queueFamilyIndices = mPhysicalDevice->getQueueFamilyIndices();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = getDeviceQueueCreateInfos(queueFamilyIndices);
    if (queueCreateInfos.empty())
    {
        CHECK_MSG(false,"Could not get any device queue create infos");
        return false;
    }
    if (!createDevice(queueCreateInfos))
    {
        CHECK_MSG(false,"Could not create Vulkan device");
        return false;
    }
    LOG("Created Vulkan device");

    if (!findDeviceQueues(queueFamilyIndices))
    {
        CHECK_MSG(false,"Could not find any Vulkan device queues");
        return false;
    }
    LOG("Found Vulkan queues");
    return true;
}

void GPUDevice::terminate() const
{
    vkDestroyDevice(mDevice, ALLOCATOR);
    LOG("Destroyed Vulkan device");
}

void GPUDevice::waitUntilIdle() const
{
    vkDeviceWaitIdle(mDevice);
}

std::vector<VkDeviceQueueCreateInfo> GPUDevice::getDeviceQueueCreateInfos(const GPUQueueFamilyIndices& queueFamilyIndices) const
{
    constexpr float queuePriority = 1.0f;
    std::set<uint32_t> queueFamilies =
    {
            queueFamilyIndices.GraphicsFamily.value(),
            queueFamilyIndices.PresentationFamily.value()
    };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t queueFamily : queueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfo.queueCount = 1;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    return queueCreateInfos;
}

bool GPUDevice::createDevice(const std::vector<VkDeviceQueueCreateInfo>& deviceQueueCreateInfos)
{
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &mPhysicalDevice->getFeatures();
    createInfo.enabledExtensionCount = mPhysicalDevice->getExtensions().size();
    createInfo.ppEnabledExtensionNames = mPhysicalDevice->getExtensions().data();
    createInfo.queueCreateInfoCount = deviceQueueCreateInfos.size();
    createInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
    if (mVulkan->isValidationLayersEnabled())
    {
        createInfo.enabledLayerCount = mVulkan->getValidationLayers().size();
        createInfo.ppEnabledLayerNames = mVulkan->getValidationLayers().data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }
    return vkCreateDevice(mPhysicalDevice->getPhysicalDevice(), &createInfo, ALLOCATOR, &mDevice) == VK_SUCCESS;
}

bool GPUDevice::findDeviceQueues(const GPUQueueFamilyIndices& queueFamilyIndices)
{
    mGraphicsQueue = findDeviceQueue(queueFamilyIndices.GraphicsFamily.value());
    if (mGraphicsQueue == VK_NULL_HANDLE)
    {
        CHECK_MSG(false,"Could not get Vulkan graphics queue");
        return false;
    }
    mPresentQueue = findDeviceQueue(queueFamilyIndices.PresentationFamily.value());
    if (mPresentQueue == VK_NULL_HANDLE)
    {
        CHECK_MSG(false,"Could not get Vulkan present queue");
        return false;
    }
    return true;
}

VkQueue GPUDevice::findDeviceQueue(uint32_t queueFamilyIndex) const
{
    constexpr int queueIndex = 0;
    VkQueue queue;
    vkGetDeviceQueue(mDevice, queueFamilyIndex, queueIndex, &queue);
    return queue;
}