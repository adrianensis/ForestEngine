#pragma once

#include <vulkan/vulkan.h>
#include "vulkan/vulkan.hpp"

#include "Core/Minimal.hpp"

class VulkanConfig
{
public:
    std::string Name;
    u32 MajorVersion = 1;
    u32 MinorVersion = 0;
    u32 PatchVersion = 0;
    bool ValidationLayersEnabled = true;
    std::vector<const char*> mRequiredExtensions;
    std::vector<const char*> mOptionalExtensions;
};

class GPUVulkanInstance
{
public:
    GPUVulkanInstance(const VulkanConfig& config);
    VkInstance getVkInstance() const;
    const std::vector<const char*>& getValidationLayers() const;
    bool isValidationLayersEnabled() const;
    bool init();
    void terminate();
    bool isExtensionAvailable(const char* extension) const;

private:
    bool createInstance();
    void destroyInstance();
    bool createDebugMessenger();
    void destroyDebugMessenger();
    std::vector<const char*> findExtensions() const;
    std::vector<VkExtensionProperties> findAvailableExtensions() const;
    bool hasExtensions(const std::vector<const char*>& extensions, std::vector<const char*>& extensionsFound) const;
    std::vector<const char*> findValidationLayers() const;
    std::vector<VkLayerProperties> findAvailableValidationLayers() const;
    bool hasValidationLayers(const std::vector<const char*>& validationLayers, const std::vector<VkLayerProperties>& availableValidationLayers) const;
    VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() const;

private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

private:
    VulkanConfig config;
    std::vector<const char*> validationLayers{};
    VkInstance mVkInstance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
    std::vector<VkExtensionProperties> availableExtensions;
};