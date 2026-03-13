#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"
#include "GPU/Descriptors/GPUDescriptorPool.hpp"
#include "GPU/Texture/GPUTextureManager.hpp"

class GPUDescriptorSet
{    
public:
    void init(const GPUDescriptorLayoutData& gpuDescriptorLayoutData, const GPUDescriptorPool& gpuDescriptorPool, GPUTextureManager* gpuTextureManager, GPUContext* gpuContext);
    void terminate();
    void updateBindlessSlot(const GPUTextureHandle& textureHandle);
private:
    void update();
    GPUContext* mGPUContext = nullptr;
    GPUTextureManager* mGPUTextureManager = nullptr;
public:
    std::vector<VkDescriptorSet> descriptorSets;
    GPUDescriptorLayout mGPUDescriptorLayout;
};

