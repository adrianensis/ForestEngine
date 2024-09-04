#include "Graphics/GPU/GPUProgramModule.h"


#include "Core/Minimal.hpp"
//namespace GPUAPI {

    ;

    GPUProgramModule::GPUProgramModule(Ptr<GPUContext> gpuContext) : mGPUContext(gpuContext) {
    }

    const VkShaderModule GPUProgramModule::getShaderModule() const {
        return shaderModule;
    }

    bool GPUProgramModule::initialize(const std::vector<byte>& moduleContent) {
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

    void GPUProgramModule::terminate() {
        vkDestroyShaderModule(mGPUContext->vulkanDevice->getDevice(), shaderModule, ALLOCATOR);
        LOG("Destroyed Vulkan shader module");
    }


// }