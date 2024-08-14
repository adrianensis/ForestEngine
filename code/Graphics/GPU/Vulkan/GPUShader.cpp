#include "GPUShader.h"
#include "Log.h"

namespace GPUAPI {

    const VkAllocationCallbacks* GPUShader::ALLOCATOR = VK_NULL_HANDLE;

    GPUShader::GPUShader(GPUDevice* vulkanDevice) : vulkanDevice(vulkanDevice) {
    }

    const VkShaderModule GPUShader::getShaderModule() const {
        return shaderModule;
    }

    bool GPUShader::initialize(const std::vector<char>& bytes) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytes.size();
        createInfo.pCode = (const uint32_t*) bytes.data();

        if (vkCreateShaderModule(vulkanDevice->getDevice(), &createInfo, ALLOCATOR, &shaderModule) != VK_SUCCESS) {
            VD_LOG_ERROR("Could not create Vulkan shader module");
            return false;
        }

        VD_LOG_INFO("Created Vulkan shader module");
        return true;
    }

    void GPUShader::terminate() {
        vkDestroyShaderModule(vulkanDevice->getDevice(), shaderModule, ALLOCATOR);
        VD_LOG_INFO("Destroyed Vulkan shader module");
    }


}