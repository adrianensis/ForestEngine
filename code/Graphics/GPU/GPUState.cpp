#include "Graphics/GPU/GPUState.hpp"

void GPUState::init()
{
    mMaxSharedBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::UNIFORM);
    mMaxSharedBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::STORAGE);

}

u32 GPUState::requestSharedBufferBindingPoint(GPUBufferType gpuSharedBufferType)
{
    u32 bindingPoint = 0;
    switch (gpuSharedBufferType)
    {
    case GPUBufferType::UNIFORM:
        {
            bindingPoint = mBindingPointsIndexUniform;
            mBindingPointsIndexUniform++;
            CHECK_MSG((i32)mBindingPointsIndexUniform <= mMaxSharedBufferBindingPointsUniform, "Max Uniform Binding Points reached!");
        }
        break;
    case GPUBufferType::STORAGE:
        {
            bindingPoint = mBindingPointsIndexStorage;
            mBindingPointsIndexStorage++;
            CHECK_MSG((i32)mBindingPointsIndexStorage <= mMaxSharedBufferBindingPointsStorage, "Max Storage Binding Points reached!");
        }
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    return bindingPoint;
}

void GPUState::terminate()
{
    mGPUSharedBuffersContainer.terminate();
}