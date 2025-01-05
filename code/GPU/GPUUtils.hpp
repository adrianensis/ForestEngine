#pragma once

#include "GPU/Core/GPUContext.hpp"

#define GPU_LOAD_EXTENSION_FUNCTION(gpuContext, extensionFunctionName) \
GPUUtils::loadExtensionFunction<PFN_##extensionFunctionName>(gpuContext, TO_STRING(extensionFunctionName));

class GPUUtils
{
public:

    template<class T>
    static T loadExtensionFunction(WeakPtr<GPUContext> gpuContext, const char* extensionFunctionName)
    {
        VULKAN_LOG("Loading Extension Function: ")
        VULKAN_LOG(extensionFunctionName)
        const char* functionName = TO_STRING(extensionFunctionName);\
        auto function = (T) vkGetInstanceProcAddr(gpuContext->vulkan->getGPUInstance(), functionName);
        if (function == nullptr)
        {
            CHECK_MSG(false, "Could not look up address of extension function!")
        }
        return function;
    }

    static VkCommandBuffer beginSingleTimeCommands(WeakPtr<GPUContext> gpuContext);
    static void endSingleTimeCommands(WeakPtr<GPUContext> gpuContext, VkCommandBuffer commandBuffer);
    static bool hasStencilComponent(WeakPtr<GPUContext> gpuContext, VkFormat format);
    static bool initializeSyncObjects(WeakPtr<GPUContext> gpuContext);

    static void drawIndexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance);
};
