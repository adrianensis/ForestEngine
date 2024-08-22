#include "Vulkan.h"

#include <utility>
#include <cstring>

#include "Core/Minimal.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        CHECK_MSG(false,pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        //VD_LOG_WARN(pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        LOG(pCallbackData->pMessage);
    } else {
        //VD_LOG_TRACE(pCallbackData->pMessage);
    }
    return VK_FALSE;
}

const VkAllocationCallbacks* Vulkan::ALLOCATOR = VK_NULL_HANDLE;

Vulkan::Vulkan(Config config, Window* window) : config(std::move(config)), window(window) {
}

VkInstance Vulkan::getGPUInstance() const {
    return vulkanInstance;
}

VkSurfaceKHR Vulkan::getSurface() const {
    return surface;
}

const std::vector<const char*>& Vulkan::getValidationLayers() const {
    return validationLayers;
}

bool Vulkan::isValidationLayersEnabled() const {
    return config.ValidationLayersEnabled;
}

bool Vulkan::initialize() {
    if (config.ValidationLayersEnabled) {
        validationLayers = findValidationLayers();
        if (validationLayers.empty()) {
            CHECK_MSG(false,"Could not get validation layers");
            return false;
        }
    }
    if (!createInstance()) {
        CHECK_MSG(false,"Could not create Vulkan instance");
        return false;
    }
    LOG("Created Vulkan instance");
    if (config.ValidationLayersEnabled) {
        if (!createDebugMessenger()) {
            CHECK_MSG(false,"Could not create debug messenger");
            return false;
        }
        LOG("Created Vulkan debug messenger");
    }
    if (!createSurface()) {
        CHECK_MSG(false,"Could not create Vulka window surface");
        return false;
    }
    LOG("Initialized Vulkan");
    return true;
}

void Vulkan::terminate() {
    destroySurface();
    if (config.ValidationLayersEnabled) {
        destroyDebugMessenger();
    }
    destroyInstance();
}

bool Vulkan::createInstance() {
    const std::vector<const char*>& extensions = findExtensions();
    if (extensions.empty()) {
        CHECK_MSG(false,"Could not get extensions");
        return false;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = config.Name.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(config.MajorVersion, config.MinorVersion, config.PatchVersion);
    appInfo.pEngineName = config.Name.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(config.MajorVersion, config.MinorVersion, config.PatchVersion);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    if (Environment::mPlatform == Environment::Platform::MACOS) {
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
    if (config.ValidationLayersEnabled) {
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = getDebugMessengerCreateInfo();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugMessengerCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    return vkCreateInstance(&createInfo, ALLOCATOR, &vulkanInstance) == VK_SUCCESS;
}

void Vulkan::destroyInstance() {
    vkDestroyInstance(vulkanInstance, ALLOCATOR);
    LOG("Destroyed Vulkan instance");
}

bool Vulkan::createDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();
    const char* functionName = "vkCreateDebugUtilsMessengerEXT";
    auto function = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, functionName);
    if (function == nullptr) {
        CHECK_MSG(false,"Could not look up address of extension function [{0}]");
        return false;
    }
    return function(vulkanInstance, &createInfo, ALLOCATOR, &debugMessenger) == VK_SUCCESS;
}

void Vulkan::destroyDebugMessenger() {
    const char* functionName = "vkDestroyDebugUtilsMessengerEXT";
    auto function = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, functionName);
    if (function == nullptr) {
        //VD_LOG_WARN("Could not look up address of extension function [{0}]", functionName);
        return;
    }
    function(vulkanInstance, debugMessenger, ALLOCATOR);
    LOG("Destroyed Vulkan debug messenger");
}

bool Vulkan::createSurface() const {
    return glfwCreateWindowSurface(vulkanInstance, window->getGlfwWindow(), ALLOCATOR, (VkSurfaceKHR*) &surface) == VK_SUCCESS;
}

void Vulkan::destroySurface() const {
    vkDestroySurfaceKHR(vulkanInstance, surface, ALLOCATOR);
    LOG("Destroyed Vulkan window surface");
}

std::vector<const char*> Vulkan::findExtensions() const {
    std::vector<const char*> requiredExtensions = findRequiredExtensions();
    //VD_LOG_DEBUG("Required extensions [{0}]", requiredExtensions.size());
    for (const char* extension: requiredExtensions) {
        //VD_LOG_DEBUG(extension);
    }
    const std::vector<VkExtensionProperties>& availableExtensions = findAvailableExtensions();
    //VD_LOG_DEBUG("Available extensions [{0}]", availableExtensions.size());
    for (const VkExtensionProperties& extensionProperties: availableExtensions) {
        //VD_LOG_DEBUG(extensionProperties.extensionName);
    }
    if (!hasExtensions(requiredExtensions, availableExtensions)) {
        CHECK_MSG(false,"Could not find required extensions");
        return {};
    }
    return requiredExtensions;
}

std::vector<const char*> Vulkan::findRequiredExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (Environment::mPlatform == Environment::Platform::MACOS) {
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }
    if (config.ValidationLayersEnabled) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}

std::vector<VkExtensionProperties> Vulkan::findAvailableExtensions() const {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    return extensions;
}

bool Vulkan::hasExtensions(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions) const {
    for (const char* extension: extensions) {
        bool extensionFound = false;
        for (const VkExtensionProperties& availableExtension: availableExtensions) {
            if (strcmp(extension, availableExtension.extensionName) == 0) {
                extensionFound = true;
                break;
            }
        }
        if (!extensionFound) {
            //VD_LOG_WARN("Could not find extension [{0}]", extension);
            return false;
        }
    }
    return true;
}

std::vector<const char*> Vulkan::findValidationLayers() const {
    std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    //VD_LOG_DEBUG("Requested validation layers [{0}]", validationLayers.size());
    for (const char* validationLayer: validationLayers) {
        //VD_LOG_DEBUG(validationLayer);
    }
    const std::vector<VkLayerProperties>& availableValidationLayers = findAvailableValidationLayers();
    //VD_LOG_DEBUG("Available validation layers [{0}]", availableValidationLayers.size());
    for (const VkLayerProperties& layerProperties: availableValidationLayers) {
        //VD_LOG_DEBUG(layerProperties.layerName);
    }
    if (!hasValidationLayers(validationLayers, availableValidationLayers)) {
        CHECK_MSG(false,"Could not find requested validation layers");
        return {};
    }
    return validationLayers;
}

std::vector<VkLayerProperties> Vulkan::findAvailableValidationLayers() const {
    uint32_t validationLayerCount;
    vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);
    std::vector<VkLayerProperties> validationLayers(validationLayerCount);
    vkEnumerateInstanceLayerProperties(&validationLayerCount, validationLayers.data());
    return validationLayers;
}

bool Vulkan::hasValidationLayers(const std::vector<const char*>& validationLayers, const std::vector<VkLayerProperties>& availableValidationLayers) const {
    for (const char* layerName: validationLayers) {
        bool layerFound = false;
        for (const auto& availableLayer: availableValidationLayers) {
            if (strcmp(layerName, availableLayer.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            //VD_LOG_WARN("Could not find validation layer [{0}]", layerName);
            return false;
        }
    }
    return true;
}

VkDebugUtilsMessengerCreateInfoEXT Vulkan::getDebugMessengerCreateInfo() const {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
    return createInfo;
}