#include "Graphics/GPU/Shader/GPUShaderModule.h"

GPUShaderModule::GPUShaderModule(Ptr<GPUContext> gpuContext) : mGPUContext(gpuContext) {}

bool GPUShaderModule::initialize(const std::vector<byte>& moduleContent) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = moduleContent.size();
    createInfo.pCode = (const uint32_t*) moduleContent.data();

    if (vkCreateShaderModule(mGPUContext->vulkanDevice->getDevice(), &createInfo, ALLOCATOR, &mShaderModule) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan shader module");
        return false;
    }

    LOG("Created Vulkan shader module");
    return true;
}

void GPUShaderModule::terminate() {
    vkDestroyShaderModule(mGPUContext->vulkanDevice->getDevice(), mShaderModule, ALLOCATOR);
    LOG("Destroyed Vulkan shader module");
}