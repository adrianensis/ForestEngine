#pragma once

#include "Graphics/GPU/GPUDevice.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUProgramModule {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        GPUDevice* vulkanDevice;
        VkShaderModule shaderModule = VK_NULL_HANDLE;

    public:
        explicit GPUProgramModule(GPUDevice* vulkanDevice);

        const VkShaderModule getShaderModule() const;

        bool initialize(const std::vector<byte>& moduleContent);

        void terminate();
    };

// }


