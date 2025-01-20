#include "GPU/Shader/GPUShaderModule.h"

bool GPUShaderModule::init(Ptr<GPUContext> gpuContext, const GPUShaderModuleData& gpuShaderModuleData)
{
    mGPUContext = gpuContext;

    SPIRVBinary spirvBinary = GPUShaderCompiler::compileShaderToSPIRV(gpuShaderModuleData.mStage, gpuShaderModuleData.mModuleContent.data(), gpuShaderModuleData.id.get().data());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvBinary.mSize * sizeof(u32);
    createInfo.pCode = spirvBinary.mWords;

    if (vkCreateShaderModule(mGPUContext->vulkanDevice->getDevice(), &createInfo, ALLOCATOR, &mShaderModule) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan shader module");
        return false;
    }

    LOG("Created Vulkan shader module");
    GPUShaderCompiler::deleteSPIRVBinary(spirvBinary);
    return true;
}

void GPUShaderModule::terminate()
{
    vkDestroyShaderModule(mGPUContext->vulkanDevice->getDevice(), mShaderModule, ALLOCATOR);
    LOG("Destroyed Vulkan shader module");
}