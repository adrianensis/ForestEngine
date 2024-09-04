#include "Graphics/GPU/Shader/GPUShaderModule.h"


#include "Core/Minimal.hpp"
//namespace GPUAPI {

    ;

    GPUShaderModule::GPUShaderModule(Ptr<GPUContext> gpuContext) : mGPUContext(gpuContext) {
    }

    const VkShaderModule GPUShaderModule::getShaderModule() const {
        return shaderModule;
    }

    bool GPUShaderModule::initialize(const std::vector<byte>& moduleContent) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = moduleContent.size();
        createInfo.pCode = (const uint32_t*) moduleContent.data();

        if (vkCreateShaderModule(mGPUContext->vulkanDevice->getDevice(), &createInfo, ALLOCATOR, &shaderModule) != VK_SUCCESS) {
            CHECK_MSG(false,"Could not create Vulkan shader module");
            return false;
        }

        LOG("Created Vulkan shader module");
        return true;
    }

    void GPUShaderModule::terminate() {
        vkDestroyShaderModule(mGPUContext->vulkanDevice->getDevice(), shaderModule, ALLOCATOR);
        LOG("Destroyed Vulkan shader module");
    }


// }