#include "GPUVulkanInstance.h"

#include <utility>
#include <cstring>
#include <cstdlib>

#include "GPU/Core/GPULog.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{  
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        GPU_LOG_TAG("DEBUG ERROR", pCallbackData->pMessage);
        CHECK_MSG(false, "VULKAN ASSERT");
    break;    
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        GPU_LOG_TAG("DEBUG WARNING", pCallbackData->pMessage);
        CHECK_MSG(false, "VULKAN ASSERT");
    break;    
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        GPU_LOG_TAG("DEBUG INFO", pCallbackData->pMessage);
    break;    
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        GPU_LOG_TAG("DEBUG VERBOSE", pCallbackData->pMessage);
    break;
    default:
        GPU_LOG_TAG("DEBUG UNKNOWN", pCallbackData->pMessage);
        break;
    }

    return VK_FALSE;
}

GPUVulkanInstance::GPUVulkanInstance(const VulkanConfig& config) : config(config) {
}

VkInstance GPUVulkanInstance::getVkInstance() const {
    return mVkInstance;
}

const std::vector<const char*>& GPUVulkanInstance::getValidationLayers() const {
    return validationLayers;
}

bool GPUVulkanInstance::isValidationLayersEnabled() const {
    return config.ValidationLayersEnabled;
}

bool GPUVulkanInstance::init()
{
#ifdef ENGINE_PLATFORM_LINUX
    setenv("VK_DRIVER_FILES", "/usr/share/vulkan/icd.d/radeon_icd.json", true);
#endif

    if (config.ValidationLayersEnabled) {
        debugMessengerCreateInfo = getDebugMessengerCreateInfo();
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
    GPU_LOG("Created Vulkan instance");
    if (config.ValidationLayersEnabled) {
        if (!createDebugMessenger()) {
            CHECK_MSG(false,"Could not create debug messenger");
            return false;
        }
        GPU_LOG("Created Vulkan debug messenger");
    }
    GPU_LOG("Initialized Vulkan");
    return true;
}

void GPUVulkanInstance::terminate() {
    if (config.ValidationLayersEnabled) {
        destroyDebugMessenger();
    }
    destroyInstance();
}

bool GPUVulkanInstance::createInstance() {
    availableExtensions = findAvailableExtensions();
    const std::vector<const char*> extensions = findExtensions();
    if (extensions.empty()) {
        CHECK_MSG(false,"Could not get extensions");
        return false;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = config.Name.c_str();
    appInfo.applicationVersion = 0;//VK_MAKE_VERSION(config.MajorVersion, config.MinorVersion, config.PatchVersion);
    appInfo.pEngineName = config.Name.c_str();
    appInfo.engineVersion = 0;//VK_MAKE_VERSION(config.MajorVersion, config.MinorVersion, config.PatchVersion);
    appInfo.apiVersion = VK_API_VERSION_1_1;
    appInfo.pNext = nullptr;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.flags = 0;
    if (Core::Environment::mPlatform == Core::Environment::Platform::MACOS)
    {
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.pNext = nullptr;

    GPU_LOG("ValidationLayersEnabled {}", config.ValidationLayersEnabled);
    if (config.ValidationLayersEnabled)
    {
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugMessengerCreateInfo;
    }

    GPU_LOG("Creating Instance");
    VkResult result = vkCreateInstance(&createInfo, ALLOCATOR, &mVkInstance);
    GPU_LOG("Instance created: ", result == VK_SUCCESS);
    return result == VK_SUCCESS;
}

void GPUVulkanInstance::destroyInstance() {
    vkDestroyInstance(mVkInstance, ALLOCATOR);
    GPU_LOG("Destroyed Vulkan instance");
}

bool GPUVulkanInstance::createDebugMessenger() {
    const char* functionName = "vkCreateDebugUtilsMessengerEXT";
    auto function = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(mVkInstance, functionName);
    if (function == nullptr) {
        CHECK_MSG(false, "Could not look up address of extension function " + std::string(functionName));
        return false;
    }
    return function(mVkInstance, &debugMessengerCreateInfo, ALLOCATOR, &debugMessenger) == VK_SUCCESS;
}

void GPUVulkanInstance::destroyDebugMessenger() {
    const char* functionName = "vkDestroyDebugUtilsMessengerEXT";
    auto function = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(mVkInstance, functionName);
    if (function == nullptr) {
        CHECK_MSG(false, "Could not look up address of extension function " + std::string(functionName));
        return;
    }
    function(mVkInstance, debugMessenger, ALLOCATOR);
    GPU_LOG("Destroyed Vulkan debug messenger");
}

std::vector<const char*> GPUVulkanInstance::findExtensions() const
{
    GPU_LOG("Required extensions " + std::to_string(config.mRequiredExtensions.size()));
    for (const char* extension: config.mRequiredExtensions) {
        GPU_LOG(extension);
    }
    GPU_LOG("Optional extensions " + std::to_string(config.mOptionalExtensions.size()));
    for (const char* extension: config.mOptionalExtensions) {
        GPU_LOG(extension);
    }
    GPU_LOG("Available extensions " + std::to_string(availableExtensions.size()));
    for (const VkExtensionProperties& extensionProperties: availableExtensions) {
        GPU_LOG(extensionProperties.extensionName);
    }

    std::vector<const char*> extensionsFound;
    bool requiredExtensionsFound = hasExtensions(config.mRequiredExtensions, extensionsFound);
    GPU_LOG("Required Extensions found: {}", requiredExtensionsFound);
    if (!requiredExtensionsFound)
    {
        CHECK_MSG(false, "Couldn't get all Required extensions.");
    }
    
    bool optionalExtensionsFound = hasExtensions(config.mOptionalExtensions, extensionsFound);
    GPU_LOG("Optional Extensions found: {}", requiredExtensionsFound);

    return extensionsFound;
}

std::vector<VkExtensionProperties> GPUVulkanInstance::findAvailableExtensions() const {
    Core::u32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    return extensions;
}

bool GPUVulkanInstance::hasExtensions(const std::vector<const char*>& extensions, std::vector<const char*>& extensionsFound) const {
    bool foundAll = true;
    for (const char* extension: extensions) {
        bool extensionFound = isExtensionAvailable(extension);
        if (extensionFound)
        {
            extensionsFound.push_back(extension);
        }
        else
        {
            GPU_LOG_WARNING("Could not find extension "s + extension);
            foundAll = false;
        }
    }
    return foundAll;
}

bool GPUVulkanInstance::isExtensionAvailable(const char* extension) const {
    bool extensionFound = false;
    for (const VkExtensionProperties& availableExtension: availableExtensions) {
        if (strcmp(extension, availableExtension.extensionName) == 0) {
            extensionFound = true;
            break;
        }
    }
    return extensionFound;
}

std::vector<const char*> GPUVulkanInstance::findValidationLayers() const {
    std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    GPU_LOG("Requested validation layers " + std::to_string(validationLayers.size()));
    for (const char* validationLayer: validationLayers) {
        GPU_LOG(validationLayer);
    }
    const std::vector<VkLayerProperties>& availableValidationLayers = findAvailableValidationLayers();
    GPU_LOG("Available validation layers " + std::to_string(availableValidationLayers.size()));
    for (const VkLayerProperties& layerProperties: availableValidationLayers) {
        GPU_LOG(layerProperties.layerName);
    }
    if (!hasValidationLayers(validationLayers, availableValidationLayers)) {
        CHECK_MSG(false,"Could not find requested validation layers");
        return {};
    }
    return validationLayers;
}

std::vector<VkLayerProperties> GPUVulkanInstance::findAvailableValidationLayers() const {
    Core::u32 validationLayerCount;
    vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);
    std::vector<VkLayerProperties> validationLayers(validationLayerCount);
    vkEnumerateInstanceLayerProperties(&validationLayerCount, validationLayers.data());
    return validationLayers;
}

bool GPUVulkanInstance::hasValidationLayers(const std::vector<const char*>& validationLayers, const std::vector<VkLayerProperties>& availableValidationLayers) const {
    for (const char* layerName: validationLayers) {
        bool layerFound = false;
        for (const auto& availableLayer: availableValidationLayers) {
            if (strcmp(layerName, availableLayer.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            GPU_LOG_WARNING("Could not find validation layer " + std::string(layerName));
            return false;
        }
    }
    return true;
}

VkDebugUtilsMessengerCreateInfoEXT GPUVulkanInstance::getDebugMessengerCreateInfo() const {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
    return createInfo;
}