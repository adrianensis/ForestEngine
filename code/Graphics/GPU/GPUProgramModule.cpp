#include "Graphics/GPU/GPUProgramModule.h"


#include "Core/Minimal.hpp"
//namespace GPUAPI {

    const VkAllocationCallbacks* GPUProgramModule::ALLOCATOR = VK_NULL_HANDLE;

    GPUProgramModule::GPUProgramModule(GPUDevice* vulkanDevice) : vulkanDevice(vulkanDevice) {
    }

    const VkShaderModule GPUProgramModule::getShaderModule() const {
        return shaderModule;
    }

    bool GPUProgramModule::initialize(const std::vector<char>& bytes) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytes.size();
        createInfo.pCode = (const uint32_t*) bytes.data();

        if (vkCreateShaderModule(vulkanDevice->getDevice(), &createInfo, ALLOCATOR, &shaderModule) != VK_SUCCESS) {
            CHECK_MSG(false,"Could not create Vulkan shader module");
            return false;
        }

        LOG("Created Vulkan shader module");
        return true;
    }

    void GPUProgramModule::terminate() {
        vkDestroyShaderModule(vulkanDevice->getDevice(), shaderModule, ALLOCATOR);
        LOG("Destroyed Vulkan shader module");
    }


// }