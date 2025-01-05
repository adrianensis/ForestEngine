#pragma once

#include "GPU/Core/GPUContext.hpp"

class GPUUtils
{
public:
    static VkCommandBuffer beginSingleTimeCommands(WeakPtr<GPUContext> gpuContext);
    static void endSingleTimeCommands(WeakPtr<GPUContext> gpuContext, VkCommandBuffer commandBuffer);
    static bool hasStencilComponent(WeakPtr<GPUContext> gpuContext, VkFormat format);
    static bool initializeSyncObjects(WeakPtr<GPUContext> gpuContext);

    static void drawIndexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance);
};
