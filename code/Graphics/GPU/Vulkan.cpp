#include "Vulkan.h"

#include <utility>
#include <cstring>
#include <cstdlib>

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUWindow.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{  
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        VULKAN_LOG_TAG("DEBUG ERROR", pCallbackData->pMessage);
        CHECK_MSG(false,pCallbackData->pMessage);
    break;    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        VULKAN_LOG_TAG("DEBUG WARNING", pCallbackData->pMessage);
        CHECK_MSG(false,pCallbackData->pMessage);
    break;    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        VULKAN_LOG_TAG("DEBUG INFO", pCallbackData->pMessage);
    break;    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        VULKAN_LOG_TAG("DEBUG INFO", pCallbackData->pMessage);
    break;
    default:
        VULKAN_LOG_TAG("DEBUG UNKNOWN", pCallbackData->pMessage);
        break;
    }

    return VK_FALSE;
}

const VkAllocationCallbacks* Vulkan::ALLOCATOR = VK_NULL_HANDLE;

Vulkan::Vulkan(Config config, GPUWindow* gpuWindow) : config(std::move(config)), gpuWindow(gpuWindow) {
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

bool Vulkan::initialize()
{
#ifdef ENGINE_PLATFORM_LINUX
    setenv("VK_DRIVER_FILES", "/usr/share/vulkan/icd.d/nvidia_icd.json", true);
#endif

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
    VULKAN_LOG("Created Vulkan instance");
    if (config.ValidationLayersEnabled) {
        if (!createDebugMessenger()) {
            CHECK_MSG(false,"Could not create debug messenger");
            return false;
        }
        VULKAN_LOG("Created Vulkan debug messenger");
    }
    if (!createSurface()) {
        CHECK_MSG(false,"Could not create Vulka gpuWindow surface");
        return false;
    }
    VULKAN_LOG("Initialized Vulkan");
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
    VULKAN_LOG("Destroyed Vulkan instance");
}

bool Vulkan::createDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();
    const char* functionName = "vkCreateDebugUtilsMessengerEXT";
    auto function = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, functionName);
    if (function == nullptr) {
        CHECK_MSG(false, "Could not look up address of extension function " + std::string(functionName));
        return false;
    }
    return function(vulkanInstance, &createInfo, ALLOCATOR, &debugMessenger) == VK_SUCCESS;
}

void Vulkan::destroyDebugMessenger() {
    const char* functionName = "vkDestroyDebugUtilsMessengerEXT";
    auto function = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, functionName);
    if (function == nullptr) {
        CHECK_MSG(false, "Could not look up address of extension function " + std::string(functionName));
        return;
    }
    function(vulkanInstance, debugMessenger, ALLOCATOR);
    VULKAN_LOG("Destroyed Vulkan debug messenger");
}

bool Vulkan::createSurface() const {
    return glfwCreateWindowSurface(vulkanInstance, gpuWindow->getGlfwWindow(), ALLOCATOR, (VkSurfaceKHR*) &surface) == VK_SUCCESS;
}

void Vulkan::destroySurface() const {
    vkDestroySurfaceKHR(vulkanInstance, surface, ALLOCATOR);
    VULKAN_LOG("Destroyed Vulkan gpuWindow surface");
}

std::vector<const char*> Vulkan::findExtensions() const {
    std::vector<const char*> requiredExtensions = findRequiredExtensions();
    VULKAN_LOG("Required extensions " + std::to_string(requiredExtensions.size()));
    for (const char* extension: requiredExtensions) {
        VULKAN_LOG(extension);
    }
    const std::vector<VkExtensionProperties>& availableExtensions = findAvailableExtensions();
    VULKAN_LOG("Available extensions " + std::to_string(availableExtensions.size()));
    for (const VkExtensionProperties& extensionProperties: availableExtensions) {
        VULKAN_LOG(extensionProperties.extensionName);
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
            VULKAN_LOG_WARNING("Could not find extension "s + extension);
            return false;
        }
    }
    return true;
}

std::vector<const char*> Vulkan::findValidationLayers() const {
    std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    VULKAN_LOG("Requested validation layers " + std::to_string(validationLayers.size()));
    for (const char* validationLayer: validationLayers) {
        VULKAN_LOG(validationLayer);
    }
    const std::vector<VkLayerProperties>& availableValidationLayers = findAvailableValidationLayers();
    VULKAN_LOG("Available validation layers " + std::to_string(availableValidationLayers.size()));
    for (const VkLayerProperties& layerProperties: availableValidationLayers) {
        VULKAN_LOG(layerProperties.layerName);
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
            VULKAN_LOG_WARNING("Could not find validation layer " + std::string(layerName));
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