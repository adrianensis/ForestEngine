#include "GPU/GPUInstance.hpp"
#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUSwapChain.h"
#include "GPU/Core/GPUCommandPool.h"
#include "GPU/Core/GPUCommandBuffer.h"

void GPUInstance::init(Core::Ptr<IGPUWindow> gpuWindow)
{
//    mMaxUniformBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::UNIFORM);
//    mMaxUniformBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::STORAGE);

    mGPUContext = Core::OwnerPtr<GPUContext>::newObject();
    mGPUContext->init(gpuWindow);
}

Core::u32 GPUInstance::requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType)
{
    Core::u32 bindingPoint = 0;
    switch (gpuUniformBufferType)
    {
    case GPUBufferType::UNIFORM:
        {
            bindingPoint = mBindingPointsIndexUniform;
            mBindingPointsIndexUniform++;
            // CHECK_MSG((Core::i32)mBindingPointsIndexUniform <= mMaxUniformBufferBindingPointsUniform, "Max Uniform Binding Points reached!");
        }
        break;
    case GPUBufferType::STORAGE:
        {
            bindingPoint = mBindingPointsIndexStorage;
            mBindingPointsIndexStorage++;
            // CHECK_MSG((Core::i32)mBindingPointsIndexStorage <= mMaxUniformBufferBindingPointsStorage, "Max Storage Binding Points reached!");
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
    mGPUContext->terminate();
}