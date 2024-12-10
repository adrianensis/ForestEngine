#pragma once

#include "GPU/Core/GPUContext.hpp"

class GPUUtils
{
public:
    static VkCommandBuffer beginSingleTimeCommands(Ptr<GPUContext> gpuContext);
    static void endSingleTimeCommands(Ptr<GPUContext> gpuContext, VkCommandBuffer commandBuffer);
    static bool hasStencilComponent(Ptr<GPUContext> gpuContext, VkFormat format);
    static bool initializeSyncObjects(Ptr<GPUContext> gpuContext);

    static void drawIndexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance);
};
