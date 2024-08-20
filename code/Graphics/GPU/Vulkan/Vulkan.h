#pragma once

#include "Graphics/Window/Window.hpp"

// #include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace GPUAPI {

    class Vulkan {
    public:
        struct Config {
            std::string Name;
            uint32_t MajorVersion = 1;
            uint32_t MinorVersion = 0;
            uint32_t PatchVersion = 0;
            bool ValidationLayersEnabled = false;
        };

    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        Config config;
        Window* window;
        std::vector<const char*> validationLayers{};
        VkInstance vulkanInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;

    public:
        Vulkan(Config config, Window* window);

        VkInstance getGPUInstance() const;

        VkSurfaceKHR getSurface() const;

        const std::vector<const char*>& getValidationLayers() const;

        bool isValidationLayersEnabled() const;

        bool initialize();

        void terminate();

    private:
        bool createInstance();

        void destroyInstance();

        bool createDebugMessenger();

        void destroyDebugMessenger();

        bool createSurface() const;

        void destroySurface() const;

        std::vector<const char*> findExtensions() const;

        std::vector<const char*> findRequiredExtensions() const;

        std::vector<VkExtensionProperties> findAvailableExtensions() const;

        bool hasExtensions(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions) const;

        std::vector<const char*> findValidationLayers() const;

        std::vector<VkLayerProperties> findAvailableValidationLayers() const;

        bool hasValidationLayers(const std::vector<const char*>& validationLayers, const std::vector<VkLayerProperties>& availableValidationLayers) const;

        VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() const;
    };
}