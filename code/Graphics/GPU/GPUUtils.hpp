#pragma once

#include "Graphics/GPU/Core/GPUContext.hpp"

class GPUUtils
{
public:
    static VkCommandBuffer beginSingleTimeCommands(Ptr<GPUContext> gpuContext);
    static void endSingleTimeCommands(Ptr<GPUContext> gpuContext, VkCommandBuffer commandBuffer);
    static bool hasStencilComponent(Ptr<GPUContext> gpuContext, VkFormat format);
    static bool initializeSyncObjects(Ptr<GPUContext> gpuContext);
};
