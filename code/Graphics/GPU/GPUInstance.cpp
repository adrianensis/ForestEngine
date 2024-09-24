#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/GPU/Core/GPUPhysicalDevice.h"
#include "Graphics/GPU/Core/GPUDevice.h"
#include "Graphics/GPU/Core/GPUSwapChain.h"
#include "Graphics/GPU/Core/GPUCommandPool.h"
#include "Graphics/GPU/Core/GPUCommandBuffer.h"

void GPUInstance::init()
{
//    mMaxUniformBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::UNIFORM);
//    mMaxUniformBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::STORAGE);

    mGPUContext = OwnerPtr<GPUContext>::newObject();
    mGPUContext->init();

    if (!GPUUtils::initializeSyncObjects(mGPUContext))
    {
        CHECK_MSG(false, "Could not create Vulkan sync objects (semaphores & fences)");
    }
}

u32 GPUInstance::requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType)
{
    u32 bindingPoint = 0;
    switch (gpuUniformBufferType)
    {
    case GPUBufferType::UNIFORM:
        {
            bindingPoint = mBindingPointsIndexUniform;
            mBindingPointsIndexUniform++;
            // CHECK_MSG((i32)mBindingPointsIndexUniform <= mMaxUniformBufferBindingPointsUniform, "Max Uniform Binding Points reached!");
        }
        break;
    case GPUBufferType::STORAGE:
        {
            bindingPoint = mBindingPointsIndexStorage;
            mBindingPointsIndexStorage++;
            // CHECK_MSG((i32)mBindingPointsIndexStorage <= mMaxUniformBufferBindingPointsStorage, "Max Storage Binding Points reached!");
        }
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    return bindingPoint;
}

void GPUInstance::terminate()
{
    mGPUUniformBuffersContainer.terminate();

    // terminateSyncObjects
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(mGPUContext->vulkanDevice->getDevice(), mGPUContext->renderFinishedSemaphores[i], allocationCallbacks);
        vkDestroySemaphore(mGPUContext->vulkanDevice->getDevice(), mGPUContext->imageAvailableSemaphores[i], allocationCallbacks);
        vkDestroyFence(mGPUContext->vulkanDevice->getDevice(), mGPUContext->inFlightFences[i], allocationCallbacks);
    }
    LOG("Destroyed Vulkan sync objects (semaphores & fences)");

    mGPUContext->vulkanSwapChain->terminate();
    delete mGPUContext->vulkanSwapChain;
    delete mGPUContext->vulkanDevice;
    delete mGPUContext->vulkanPhysicalDevice;
    delete mGPUContext->vulkanCommandPool;
    delete mGPUContext->vulkan;
}