#include "GPUPhysicalDevice.h"
#include "Core/CoreMacros.hpp"
#include "GPU/Core/GPULog.h"
#include "vulkan/vulkan_core.h"

GPUPhysicalDevice::GPUPhysicalDevice(GPUVulkanInstance* mGPUVulkanInstance, GPUPhysicalDeviceData gpuPhysicalDeviceData) : mGPUVulkanInstance(mGPUVulkanInstance), mGPUPhysicalDeviceData(gpuPhysicalDeviceData) {
}

VkPhysicalDevice GPUPhysicalDevice::getPhysicalDevice() const {
    return deviceInfo.mPhysicalDevice;
}

const GPUDeviceInfo& GPUPhysicalDevice::getDeviceInfo() const {
    return deviceInfo;
}

const VkPhysicalDeviceProperties2& GPUPhysicalDevice::getProperties() const {
    return deviceInfo.mProperties;
}

const VkPhysicalDeviceFeatures2& GPUPhysicalDevice::getFeatures() const {
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
    findAvailableDevices(mAvailableDevices);

    if (mAvailableDevices.empty())
    {
        CHECK_MSG(false,"Could not get any available devices");
        return false;
    }
    this->deviceInfo = findMostSuitableDevice(mAvailableDevices);
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

GPU::u32 GPUPhysicalDevice::findMemoryType(GPU::u32 memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags) const
{
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(deviceInfo.mPhysicalDevice, &physicalDeviceMemoryProperties);
    for (GPU::u32 memoryTypeIndex = 0; memoryTypeIndex < physicalDeviceMemoryProperties.memoryTypeCount; memoryTypeIndex++)
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

void GPUPhysicalDevice::findAvailableDevices(std::vector<GPUDeviceInfo>& outDevices) const
{
    GPU::u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(mGPUVulkanInstance->getVkInstance(), &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> vkPhysicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(mGPUVulkanInstance->getVkInstance(), &deviceCount, vkPhysicalDevices.data());

    FOR_ARRAY(i, vkPhysicalDevices)
    {
        VkPhysicalDevice vkPhysicalDevice = vkPhysicalDevices[i];
        GPUDeviceInfo& device = outDevices.emplace_back(GPUDeviceInfo{});
        device.mPhysicalDevice = vkPhysicalDevice;
        
        device.mSubgroupProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
        device.mSubgroupProperties.pNext = nullptr;

        // Link the extension structure into the pNext chain
        device.mProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        device.mProperties.pNext = &device.mSubgroupProperties;
        vkGetPhysicalDeviceProperties2(vkPhysicalDevice, &device.mProperties);

        // extended features (for stencil dynamic op)
        device.mExtendedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
        device.mExtendedFeatures.pNext = nullptr;
        device.mExtendedFeatures.extendedDynamicState = VK_TRUE;

        // indexing features for bindless
        device.mIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        device.mIndexingFeatures.pNext = &device.mExtendedFeatures;
        device.mIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
        device.mIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
        
        // Define the specific 1.3 features (contains dynamicRendering)
        device.m13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        device.m13Features.dynamicRendering = VK_TRUE; 
        device.m13Features.pNext = &device.mIndexingFeatures; 
        device.m13Features.dynamicRendering = VK_TRUE;
        
        // Define the top-level Features2 structure
        device.mFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        device.mFeatures.pNext = &device.m13Features;
        vkGetPhysicalDeviceFeatures2(vkPhysicalDevice, &device.mFeatures);

        /*
            * There are certain limitations of our current MSAA implementation which may impact the quality of the output image in more detailed scenes.
            * For example, we're currently not solving potential problems caused by shader aliasing, i.e. MSAA only smoothens out the edges of geometry but not the interior filling.
            * This may lead to a situation when you get a smooth polygon rendered on screen but the applied texture will still look aliased if it contains high contrasting colors.
            * One way to approach this problem is to enable Sample Shading which will improve the image quality even further, though at an additional performance cost:
            */
        //vkPhysicalDeviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature for the device

        device.mExtensions = findExtensions(vkPhysicalDevice);
        device.mQueueFamilyIndices = findQueueFamilyIndices(vkPhysicalDevice);
        device.mSwapChainInfo = findSwapChainInfo(vkPhysicalDevice);
        device.mSampleCount = getSampleCount(device.mProperties);
    }
    GPU_LOG("Available physical devices: " + std::to_string(deviceCount));
    for (const GPUDeviceInfo& device : outDevices) {
        GPU_LOG(device.mProperties.properties.deviceName + " : "s + getDeviceTypeAsString(device.mProperties.properties.deviceType));
    }
}

std::vector<VkExtensionProperties> GPUPhysicalDevice::findExtensions(VkPhysicalDevice device) const
{
    const char* layerName = nullptr;

    GPU::u32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, layerName, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, layerName, &extensionCount, extensions.data());

    GPU_LOG("Available device extensions: " + std::to_string(extensions.size()));
    for (const VkExtensionProperties& extensionProperties : extensions)
    {
        GPU_LOG(extensionProperties.extensionName);
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
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            // VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME is included in 1.3, but this help us to detect missing device support.
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME
    };
    return extensions;
}

const std::vector<const char*>& GPUPhysicalDevice::getOptionalExtensions() const
{
    static std::vector<const char*> extensions =
    {
        #ifdef VK_ENABLE_BETA_EXTENSIONS
            VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        #endif
    };
    return extensions;
}

GPUQueueFamilyIndices GPUPhysicalDevice::findQueueFamilyIndices(VkPhysicalDevice device) const
{
    GPU::u32 queueFamilyCount = 0;
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
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mGPUPhysicalDeviceData.surface, &presentationSupport);
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

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mGPUPhysicalDeviceData.surface, &swapChainInfo.SurfaceCapabilities);

    GPU::u32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mGPUPhysicalDeviceData.surface, &formatCount, nullptr);
    swapChainInfo.SurfaceFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mGPUPhysicalDeviceData.surface, &formatCount, swapChainInfo.SurfaceFormats.data());

    GPU::u32 presentationModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mGPUPhysicalDeviceData.surface, &presentationModeCount, nullptr);
    swapChainInfo.PresentModes.resize(presentationModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mGPUPhysicalDeviceData.surface, &presentationModeCount, swapChainInfo.PresentModes.data());

    return swapChainInfo;
}

VkSampleCountFlagBits GPUPhysicalDevice::getSampleCount(const VkPhysicalDeviceProperties2& deviceProperties) const
{
    VkSampleCountFlags counts = deviceProperties.properties.limits.framebufferColorSampleCounts & deviceProperties.properties.limits.framebufferDepthSampleCounts;
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
    std::multimap<GPU::u32, GPUDeviceInfo> devicesByRating;
    GPU_LOG("Device suitability ratings");
    for (const GPUDeviceInfo& device : availableDevices)
    {
        GPU::u32 suitabilityRating = getSuitabilityRating(device);
        GPU_LOG(device.mProperties.properties.deviceName + " : "s + std::to_string(suitabilityRating));
        devicesByRating.insert(std::make_pair(suitabilityRating, device));
    }
    GPU::u32 highestRating = devicesByRating.rbegin()->first;
    if (highestRating == 0)
    {
        return {};
    }
    GPU_LOG("Most suitable device");
    const GPUDeviceInfo& device = devicesByRating.rbegin()->second;
    GPU_LOG(device.mProperties.properties.deviceName);
    return device;
}

GPU::u32 GPUPhysicalDevice::getSuitabilityRating(const GPUDeviceInfo& deviceInfo) const
{
    if (!hasRequiredFeatures(deviceInfo.mFeatures))
    {
        GPU_LOG(deviceInfo.mProperties.properties.deviceName + " does not have required device features"s);
        return 0;
    }
    if (!hasRequiredExtensions(deviceInfo.mExtensions))
    {
        GPU_LOG(deviceInfo.mProperties.properties.deviceName + " does not have required device extensions"s);
        return 0;
    }
    if (!hasRequiredSwapChainSupport(deviceInfo.mSwapChainInfo))
    {
        GPU_LOG(deviceInfo.mProperties.properties.deviceName + " does not have required swap chain info"s);
        return 0;
    }
    if (!hasRequiredQueueFamilyIndices(deviceInfo.mQueueFamilyIndices))
    {
        GPU_LOG(deviceInfo.mProperties.properties.deviceName + " does not have required queue family indices"s);
        return 0;
    }
    GPU::u32 score = 0;
    // Discrete GPUs have a significant performance advantage
    if (deviceInfo.mProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }
    score += (GPU::u32) deviceInfo.mProperties.properties.limits.maxImageDimension2D;
    score += (GPU::u32) deviceInfo.mProperties.properties.limits.framebufferColorSampleCounts;
    score += (GPU::u32) deviceInfo.mProperties.properties.limits.framebufferDepthSampleCounts;
    return score;
}

bool GPUPhysicalDevice::hasRequiredFeatures(const VkPhysicalDeviceFeatures2& availableDeviceFeatures) const
{
    VkPhysicalDeviceVulkan13Features* features13 = static_cast<VkPhysicalDeviceVulkan13Features*>(availableDeviceFeatures.pNext);
    VkPhysicalDeviceDescriptorIndexingFeatures* indexingFeatures = static_cast<VkPhysicalDeviceDescriptorIndexingFeatures*>(features13->pNext);
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT* extendedDynamicStateFeaturesEXT = static_cast<VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(indexingFeatures->pNext);
    return availableDeviceFeatures.features.samplerAnisotropy && 
    availableDeviceFeatures.features.shaderSampledImageArrayDynamicIndexing
    && features13 != nullptr
    && features13->dynamicRendering
    // bindless features
    && indexingFeatures
    && indexingFeatures->descriptorBindingPartiallyBound
    && indexingFeatures->runtimeDescriptorArray
    && indexingFeatures->shaderSampledImageArrayNonUniformIndexing
    && indexingFeatures->descriptorBindingSampledImageUpdateAfterBind
    && indexingFeatures->shaderUniformBufferArrayNonUniformIndexing
    && indexingFeatures->descriptorBindingUniformBufferUpdateAfterBind
    && indexingFeatures->shaderStorageBufferArrayNonUniformIndexing
    && indexingFeatures->descriptorBindingStorageBufferUpdateAfterBind
    // stencil and depth dynamic states
    && extendedDynamicStateFeaturesEXT
    && extendedDynamicStateFeaturesEXT->extendedDynamicState;
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