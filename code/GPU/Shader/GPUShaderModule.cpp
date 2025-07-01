#include "GPU/Shader/GPUShaderModule.h"

bool GPUShaderModule::init(Core::Ptr<GPUContext> gpuContext, const GPUShaderModuleData& gpuGPUShaderModuleData)
{
    mGPUContext = gpuContext;

    SPIRVBinary spirvBinary = GPUShaderCompiler::compileGPUShaderToSPIRV(gpuGPUShaderModuleData.mStage, gpuGPUShaderModuleData.mModuleContent.data(), gpuGPUShaderModuleData.id.get().data());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvBinary.mSize * sizeof(Core::u32);
    createInfo.pCode = spirvBinary.mWords;

    if (vkCreateShaderModule(mGPUContext->vulkanDevice->getDevice(), &createInfo, ALLOCATOR, &mGPUShaderModule) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan shader module");
        return false;
    }

    LOG("Created Vulkan shader module");
    GPUShaderCompiler::deleteSPIRVBinary(spirvBinary);
    return true;
}

void GPUShaderModule::terminate()
{
    vkDestroyShaderModule(mGPUContext->vulkanDevice->getDevice(), mGPUShaderModule, ALLOCATOR);
    LOG("Destroyed Vulkan shader module");
}