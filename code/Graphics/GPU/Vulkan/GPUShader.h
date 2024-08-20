#pragma once

#include "GPUDevice.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUShader {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        GPUDevice* vulkanDevice;
        VkShaderModule shaderModule = VK_NULL_HANDLE;

    public:
        explicit GPUShader(GPUDevice* vulkanDevice);

        const VkShaderModule getShaderModule() const;

        bool initialize(const std::vector<char>& bytes);

        void terminate();
    };

// }


