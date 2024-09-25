#include "GPUPhysicalDevice.h"

GPUPhysicalDevice::GPUPhysicalDevice(Vulkan* vulkan) : vulkan(vulkan) {
}

VkPhysicalDevice GPUPhysicalDevice::getPhysicalDevice() const {
    return deviceInfo.mPhysicalDevice;
}

const VkPhysicalDeviceProperties& GPUPhysicalDevice::getProperties() const {
    return deviceInfo.mProperties;
}

const VkPhysicalDeviceFeatures& GPUPhysicalDevice::getFeatures() const {
    return deviceInfo.mFeatures;
}

const GPUQueueFamilyIndices& GPUPhysicalDevice::getQueueFamilyIndices() const {
    return deviceInfo.mQueueFamilyIndices;
}

const GPUSwapChainInfo& GPUPhysicalDevice::getSwapChainInfo() const {
    return deviceInfo.mSwapChainInfo;
}

const std::vector<const char*>& GPUPhysicalDevice::getExtensions() const {
    return getRequiredExtensions();
}

VkSampleCountFlagBits GPUPhysicalDevice::getSampleCount() const {
    return deviceInfo.mSampleCount;
}

bool GPUPhysicalDevice::init()
{
    std::vector<GPUDeviceInfo> availableDevices = findAvailableDevices();
    if (availableDevices.empty())
    {
        CHECK_MSG(false,"Could not get any available devices");
        return false;
    }
    this->deviceInfo = findMostSuitableDevice(availableDevices);
    if (this->deviceInfo.mPhysicalDevice == nullptr)
    {
        CHECK_MSG(false,"Could not get any suitable device");
        return false;
    }
    LOG("Initialized Vulkan physical device");
    return true;
}

void GPUPhysicalDevice::updateSwapChainInfo()
{
    deviceInfo.mSwapChainInfo = findSwapChainInfo(deviceInfo.mPhysicalDevice);
}

uint32_t GPUPhysicalDevice::findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags) const
{
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(deviceInfo.mPhysicalDevice, &physicalDeviceMemoryProperties);
    for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < physicalDeviceMemoryProperties.memoryTypeCount; memoryTypeIndex++)
    {
        /*
            * The memoryTypeBits parameter will be used to specify the bit field of memory types that are suitable.
            * That means that we can find the index of a suitable memory type by simply iterating over them and checking if the corresponding bit is set to 1.
            */
        bool isSuitableType = (memoryTypeBits & (1 << memoryTypeIndex)) > 0;

        /*
            * However, we're not just interested in a memory type that is suitable for the buffer. We also need to ensure that it has the necessary properties
            * The memoryTypes array consists of VkMemoryType structs that specify the heap and properties of each type of memory.
            */
        VkMemoryType& memoryType = physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex];
        bool hasNecessaryProperties = (memoryType.propertyFlags & memoryPropertyFlags) == memoryPropertyFlags;

        if (isSuitableType && hasNecessaryProperties)
        {
            return memoryTypeIndex;
        }
    }
    CHECK_MSG(false, "Could not find memory type");
    return -1;
}

VkFormat GPUPhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(deviceInfo.mPhysicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }
    CHECK_MSG(false, "Could not find supported format");
    return VK_FORMAT_UNDEFINED;
}

std::vector<GPUDeviceInfo> GPUPhysicalDevice::findAvailableDevices() const
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vulkan->getGPUInstance(), &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> vkPhysicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(vulkan->getGPUInstance(), &deviceCount, vkPhysicalDevices.data());

    std::vector<GPUDeviceInfo> devices;
    for (VkPhysicalDevice vkPhysicalDevice : vkPhysicalDevices)
    {

        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkPhysicalDeviceProperties);

        VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &vkPhysicalDeviceFeatures);

        /*
            * There are certain limitations of our current MSAA implementation which may impact the quality of the output image in more detailed scenes.
            * For example, we're currently not solving potential problems caused by shader aliasing, i.e. MSAA only smoothens out the edges of geometry but not the interior filling.
            * This may lead to a situation when you get a smooth polygon rendered on screen but the applied texture will still look aliased if it contains high contrasting colors.
            * One way to approach this problem is to enable Sample Shading which will improve the image quality even further, though at an additional performance cost:
            */
        //vkPhysicalDeviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature for the device

        GPUDeviceInfo device{};
        device.mPhysicalDevice = vkPhysicalDevice;
        device.mProperties = vkPhysicalDeviceProperties;
        device.mFeatures = vkPhysicalDeviceFeatures;
        device.mExtensions = findExtensions(vkPhysicalDevice);
        device.mQueueFamilyIndices = findQueueFamilyIndices(vkPhysicalDevice);
        device.mSwapChainInfo = findSwapChainInfo(vkPhysicalDevice);
        device.mSampleCount = getSampleCount(vkPhysicalDeviceProperties);

        devices.push_back(device);
    }
    VULKAN_LOG("Available physical devices: " + std::to_string(deviceCount));
    for (const GPUDeviceInfo& device : devices) {
        VULKAN_LOG(device.mProperties.deviceName + " : "s + getDeviceTypeAsString(device.mProperties.deviceType));
    }
    return devices;
}

std::vector<VkExtensionProperties> GPUPhysicalDevice::findExtensions(VkPhysicalDevice device) const
{
    const char* layerName = nullptr;

    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, layerName, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, layerName, &extensionCount, extensions.data());

    VULKAN_LOG("Available device extensions: " + std::to_string(extensions.size()));
    for (const VkExtensionProperties& extensionProperties : extensions)
    {
        VULKAN_LOG(extensionProperties.extensionName);
        for (const char* optionalExtension : getOptionalExtensions())
        {
            if (std::strcmp(extensionProperties.extensionName, optionalExtension) == 0)
            {
                getRequiredExtensions().push_back(optionalExtension);
            }
        }
    }
    return extensions;
}

std::vector<const char*>& GPUPhysicalDevice::getRequiredExtensions() const
{
    static std::vector<const char*> extensions =
    {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    return extensions;
}

const std::vector<const char*>& GPUPhysicalDevice::getOptionalExtensions() const
{
    static std::vector<const char*> extensions =
    {
            "VK_KHR_portability_subset"
    };
    return extensions;
}

GPUQueueFamilyIndices GPUPhysicalDevice::findQueueFamilyIndices(VkPhysicalDevice device) const
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    GPUQueueFamilyIndices indices;
    for (int i = 0; i < queueFamilies.size(); i++)
    {
        const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = i;
        }
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vulkan->getSurface(), &presentationSupport);
        if (presentationSupport)
        {
            indices.PresentationFamily = i;
        }
        if (indices.GraphicsFamily.has_value() && indices.PresentationFamily.has_value())
        {
            break;
        }
    }
    return indices;
}

GPUSwapChainInfo GPUPhysicalDevice::findSwapChainInfo(VkPhysicalDevice device) const
{
    GPUSwapChainInfo swapChainInfo;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkan->getSurface(), &swapChainInfo.SurfaceCapabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->getSurface(), &formatCount, nullptr);
    swapChainInfo.SurfaceFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->getSurface(), &formatCount, swapChainInfo.SurfaceFormats.data());

    uint32_t presentationModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->getSurface(), &presentationModeCount, nullptr);
    swapChainInfo.PresentModes.resize(presentationModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->getSurface(), &presentationModeCount, swapChainInfo.PresentModes.data());

    return swapChainInfo;
}

VkSampleCountFlagBits GPUPhysicalDevice::getSampleCount(const VkPhysicalDeviceProperties& deviceProperties) const
{
    VkSampleCountFlags counts = deviceProperties.limits.framebufferColorSampleCounts & deviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
    return VK_SAMPLE_COUNT_1_BIT;
}

std::string GPUPhysicalDevice::getDeviceTypeAsString(VkPhysicalDeviceType deviceType) const
{
    switch (deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "VK_PHYSICAL_DEVICE_TYPE_OTHER";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "VK_PHYSICAL_DEVICE_TYPE_CPU";
        default:
            return "";
    }
}

GPUDeviceInfo GPUPhysicalDevice::findMostSuitableDevice(const std::vector<GPUDeviceInfo>& availableDevices) const
{
    std::multimap<uint32_t, GPUDeviceInfo> devicesByRating;
    VULKAN_LOG("Device suitability ratings");
    for (const GPUDeviceInfo& device : availableDevices)
    {
        uint32_t suitabilityRating = getSuitabilityRating(device);
        VULKAN_LOG(device.mProperties.deviceName + " : "s + std::to_string(suitabilityRating));
        devicesByRating.insert(std::make_pair(suitabilityRating, device));
    }
    uint32_t highestRating = devicesByRating.rbegin()->first;
    if (highestRating == 0)
    {
        return {};
    }
    VULKAN_LOG("Most suitable device");
    const GPUDeviceInfo& device = devicesByRating.rbegin()->second;
    VULKAN_LOG(device.mProperties.deviceName);
    return device;
}

uint32_t GPUPhysicalDevice::getSuitabilityRating(const GPUDeviceInfo& deviceInfo) const
{
    if (!hasRequiredFeatures(deviceInfo.mFeatures))
    {
        VULKAN_LOG(deviceInfo.mProperties.deviceName + " does not have required device features"s);
        return 0;
    }
    if (!hasRequiredExtensions(deviceInfo.mExtensions))
    {
        VULKAN_LOG(deviceInfo.mProperties.deviceName + " does not have required device extensions"s);
        return 0;
    }
    if (!hasRequiredSwapChainSupport(deviceInfo.mSwapChainInfo))
    {
        VULKAN_LOG(deviceInfo.mProperties.deviceName + " does not have required swap chain info"s);
        return 0;
    }
    if (!hasRequiredQueueFamilyIndices(deviceInfo.mQueueFamilyIndices))
    {
        VULKAN_LOG(deviceInfo.mProperties.deviceName + " does not have required queue family indices"s);
        return 0;
    }
    uint32_t score = 0;
    // Discrete GPUs have a significant performance advantage
    if (deviceInfo.mProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }
    score += (uint32_t) deviceInfo.mProperties.limits.maxImageDimension2D;
    score += (uint32_t) deviceInfo.mProperties.limits.framebufferColorSampleCounts;
    score += (uint32_t) deviceInfo.mProperties.limits.framebufferDepthSampleCounts;
    return score;
}

bool GPUPhysicalDevice::hasRequiredFeatures(const VkPhysicalDeviceFeatures& availableDeviceFeatures) const
{
    return availableDeviceFeatures.samplerAnisotropy;
}

bool GPUPhysicalDevice::hasRequiredExtensions(const std::vector<VkExtensionProperties>& availableDeviceExtensions) const
{
    for (const char* requiredExtension : this->getRequiredExtensions())
    {
        bool requiredExtensionFound = false;
        for (const VkExtensionProperties& availableExtension : availableDeviceExtensions)
        {
            if (std::strcmp(requiredExtension, availableExtension.extensionName) == 0)
            {
                requiredExtensionFound = true;
                break;
            }
        }
        if (!requiredExtensionFound)
        {
            CHECK_MSG(false, "Could not find required extension [{0}]");
            return false;
        }
    }
    return true;
}

bool GPUPhysicalDevice::hasRequiredSwapChainSupport(const GPUSwapChainInfo& swapChainInfo) const
{
    return !swapChainInfo.SurfaceFormats.empty() && !swapChainInfo.PresentModes.empty();
}

bool GPUPhysicalDevice::hasRequiredQueueFamilyIndices(const GPUQueueFamilyIndices& queueFamilyIndices) const
{
    return queueFamilyIndices.GraphicsFamily.has_value() && queueFamilyIndices.PresentationFamily.has_value();
}