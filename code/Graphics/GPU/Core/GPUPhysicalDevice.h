#pragma once

#include "Vulkan.h"

#include <vulkan/vulkan.h>
#include <optional>

struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentationFamily;
};

struct SwapChainInfo
{
    VkSurfaceCapabilitiesKHR SurfaceCapabilities{};
    std::vector<VkSurfaceFormatKHR> SurfaceFormats;
    std::vector<VkPresentModeKHR> PresentModes;
};

class GPUPhysicalDevice
{
private:
    struct DeviceInfo
    {
        VkPhysicalDevice mPhysicalDevice = nullptr;
        VkPhysicalDeviceProperties mProperties{};
        VkPhysicalDeviceFeatures mFeatures{};
        std::vector<VkExtensionProperties> mExtensions{};
        QueueFamilyIndices mQueueFamilyIndices{};
        SwapChainInfo mSwapChainInfo{};
        VkSampleCountFlagBits mSampleCount = VK_SAMPLE_COUNT_1_BIT;
    };

private:
    Vulkan* vulkan;
    DeviceInfo deviceInfo{};

public:
    explicit GPUPhysicalDevice(Vulkan* vulkan);
    VkPhysicalDevice getPhysicalDevice() const;
    const VkPhysicalDeviceProperties& getProperties() const;
    const VkPhysicalDeviceFeatures& getFeatures() const;
    const QueueFamilyIndices& getQueueFamilyIndices() const;
    const SwapChainInfo& getSwapChainInfo() const;
    VkSampleCountFlagBits getSampleCount() const;
    const std::vector<const char*>& getExtensions() const;
    bool init();
    void updateSwapChainInfo();
    uint32_t findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags) const;
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

private:
    std::vector<DeviceInfo> findAvailableDevices() const;
    std::vector<VkExtensionProperties> findExtensions(VkPhysicalDevice device) const;
    std::vector<const char*>& getRequiredExtensions() const;
    const std::vector<const char*>& getOptionalExtensions() const;
    QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device) const;
    SwapChainInfo findSwapChainInfo(VkPhysicalDevice device) const;
    VkSampleCountFlagBits getSampleCount(const VkPhysicalDeviceProperties& deviceProperties) const;
    std::string getDeviceTypeAsString(VkPhysicalDeviceType deviceType) const;
    DeviceInfo findMostSuitableDevice(const std::vector<DeviceInfo>& availableDevices) const;
    uint32_t getSuitabilityRating(const DeviceInfo& deviceInfo) const;
    bool hasRequiredFeatures(const VkPhysicalDeviceFeatures& availableDeviceFeatures) const;
    bool hasRequiredExtensions(const std::vector<VkExtensionProperties>& availableDeviceExtensions) const;
    bool hasRequiredSwapChainSupport(const SwapChainInfo& swapChainInfo) const;
    bool hasRequiredQueueFamilyIndices(const QueueFamilyIndices& queueFamilyIndices) const;
};