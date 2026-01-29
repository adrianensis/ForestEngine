#pragma once

#include "GPUVulkanInstance.h"

#include <vulkan/vulkan.h>
#include <optional>

class GPUQueueFamilyIndices
{
public:
    std::optional<Core::u32> GraphicsFamily;
    std::optional<Core::u32> PresentationFamily;
};

class GPUSwapChainInfo
{
public:
    VkSurfaceCapabilitiesKHR SurfaceCapabilities{};
    std::vector<VkSurfaceFormatKHR> SurfaceFormats;
    std::vector<VkPresentModeKHR> PresentModes;
};

class GPUDeviceInfo
{
public:
    VkPhysicalDevice mPhysicalDevice = nullptr;
    VkPhysicalDeviceSubgroupProperties mSubgroupProperties{};
    VkPhysicalDeviceProperties2 mProperties{};
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT mExtendedFeatures{};
    VkPhysicalDeviceVulkan13Features m13Features{};
    VkPhysicalDeviceDescriptorIndexingFeatures mIndexingFeatures{};
    VkPhysicalDeviceFeatures2 mFeatures{};
    std::vector<VkExtensionProperties> mExtensions{};
    GPUQueueFamilyIndices mQueueFamilyIndices{};
    GPUSwapChainInfo mSwapChainInfo{};
    VkSampleCountFlagBits mSampleCount = VK_SAMPLE_COUNT_1_BIT;
};

class GPUPhysicalDeviceData
{
public:
    VkSurfaceKHR surface = VK_NULL_HANDLE;
};

class GPUPhysicalDevice
{
private:
    GPUVulkanInstance* mGPUVulkanInstance;
    GPUDeviceInfo deviceInfo{};
    GPUPhysicalDeviceData mGPUPhysicalDeviceData;
    std::vector<GPUDeviceInfo> mAvailableDevices;
public:
    explicit GPUPhysicalDevice(GPUVulkanInstance* gpuVulkanInstance, GPUPhysicalDeviceData gpuPhysicalDeviceData);
    VkPhysicalDevice getPhysicalDevice() const;
    const GPUDeviceInfo& getDeviceInfo() const;
    const VkPhysicalDeviceProperties2& getProperties() const;
    const VkPhysicalDeviceFeatures2& getFeatures() const;
    const GPUQueueFamilyIndices& getQueueFamilyIndices() const;
    const GPUSwapChainInfo& getSwapChainInfo() const;
    VkSampleCountFlagBits getSampleCount() const;
    const std::vector<const char*>& getExtensions() const;
    bool init();
    void updateSwapChainInfo();
    Core::u32 findMemoryType(Core::u32 memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags) const;
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

private:
    void findAvailableDevices(std::vector<GPUDeviceInfo>& outDevices) const;
    std::vector<VkExtensionProperties> findExtensions(VkPhysicalDevice device) const;
    std::vector<const char*>& getRequiredExtensions() const;
    const std::vector<const char*>& getOptionalExtensions() const;
    GPUQueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device) const;
    GPUSwapChainInfo findSwapChainInfo(VkPhysicalDevice device) const;
    VkSampleCountFlagBits getSampleCount(const VkPhysicalDeviceProperties2& deviceProperties) const;
    std::string getDeviceTypeAsString(VkPhysicalDeviceType deviceType) const;
    GPUDeviceInfo findMostSuitableDevice(const std::vector<GPUDeviceInfo>& availableDevices) const;
    Core::u32 getSuitabilityRating(const GPUDeviceInfo& deviceInfo) const;
    bool hasRequiredFeatures(const VkPhysicalDeviceFeatures2& availableDeviceFeatures) const;
    bool hasRequiredExtensions(const std::vector<VkExtensionProperties>& availableDeviceExtensions) const;
    bool hasRequiredSwapChainSupport(const GPUSwapChainInfo& swapChainInfo) const;
    bool hasRequiredQueueFamilyIndices(const GPUQueueFamilyIndices& queueFamilyIndices) const;
};