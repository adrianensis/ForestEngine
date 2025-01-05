#pragma once

#include "Vulkan.h"

#include <vulkan/vulkan.h>
#include <optional>

class GPUQueueFamilyIndices
{
public:
    std::optional<u32> GraphicsFamily;
    std::optional<u32> PresentationFamily;
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
    VkPhysicalDeviceProperties mProperties{};
    VkPhysicalDeviceFeatures mFeatures{};
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
    Vulkan* vulkan;
    GPUDeviceInfo deviceInfo{};
    GPUPhysicalDeviceData mGPUPhysicalDeviceData;

public:
    explicit GPUPhysicalDevice(Vulkan* vulkan, GPUPhysicalDeviceData gpuPhysicalDeviceData);
    VkPhysicalDevice getPhysicalDevice() const;
    const VkPhysicalDeviceProperties& getProperties() const;
    const VkPhysicalDeviceFeatures& getFeatures() const;
    const GPUQueueFamilyIndices& getQueueFamilyIndices() const;
    const GPUSwapChainInfo& getSwapChainInfo() const;
    VkSampleCountFlagBits getSampleCount() const;
    const std::vector<const char*>& getExtensions() const;
    bool init();
    void updateSwapChainInfo();
    u32 findMemoryType(u32 memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags) const;
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

private:
    std::vector<GPUDeviceInfo> findAvailableDevices() const;
    std::vector<VkExtensionProperties> findExtensions(VkPhysicalDevice device) const;
    std::vector<const char*>& getRequiredExtensions() const;
    const std::vector<const char*>& getOptionalExtensions() const;
    GPUQueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device) const;
    GPUSwapChainInfo findSwapChainInfo(VkPhysicalDevice device) const;
    VkSampleCountFlagBits getSampleCount(const VkPhysicalDeviceProperties& deviceProperties) const;
    std::string getDeviceTypeAsString(VkPhysicalDeviceType deviceType) const;
    GPUDeviceInfo findMostSuitableDevice(const std::vector<GPUDeviceInfo>& availableDevices) const;
    u32 getSuitabilityRating(const GPUDeviceInfo& deviceInfo) const;
    bool hasRequiredFeatures(const VkPhysicalDeviceFeatures& availableDeviceFeatures) const;
    bool hasRequiredExtensions(const std::vector<VkExtensionProperties>& availableDeviceExtensions) const;
    bool hasRequiredSwapChainSupport(const GPUSwapChainInfo& swapChainInfo) const;
    bool hasRequiredQueueFamilyIndices(const GPUQueueFamilyIndices& queueFamilyIndices) const;
};